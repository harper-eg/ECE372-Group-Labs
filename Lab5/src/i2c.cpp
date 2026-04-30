// Authors:
// Net IDs:
// Date:
// Assignment: Lab 5
//
// I2C (TWI) driver for the MPU-6050 accelerometer. The function names and
// decomposition follow the Lab 5 spec:
//   InitI2C, StartI2C_Trans, StopI2C_Trans, Write, Read_from, Read_data
//
// All bus operations carry a TWINT timeout so a stuck bus (no pull-ups,
// shorted SDA/SCL, etc.) cannot lock up the rest of the firmware.
//----------------------------------------------------------------------//

#include "i2c.h"
#include <avr/io.h>
#include <stdlib.h>

// Most-recent raw accelerometer bytes. accelN1 = high, accelN0 = low.
unsigned char accelX0 = 0;
unsigned char accelX1 = 0;
unsigned char accelY0 = 0;
unsigned char accelY1 = 0;
unsigned char accelZ0 = 0;
unsigned char accelZ1 = 0;

// Tilt threshold in raw LSB units. At ±2g full scale, 1g = 16384 LSB and
// sin(45°)*16384 ≈ 11585. Tune experimentally with the serial output.
#define TILT_THRESHOLD 140

// Roughly 6-8 ms of busy-wait at 16 MHz; comfortably longer than any
// 100 kHz I²C operation but short enough that a hung bus doesn't stall
// the system noticeably.
#define TWINT_TIMEOUT 30000UL

// Wait for TWINT with a timeout. On timeout, force a STOP to try to
// recover the bus and return false so the caller can bail.
static bool wait_TWINT() {
    uint32_t timeout = TWINT_TIMEOUT;
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
            return false;
        }
    }
    return true;
}

void InitI2C() {
    PRR0 &= ~(1 << PRTWI);                  // wake the TWI module
    TWSR  = (TWSR & ~(1 << TWPS1)) | (1 << TWPS0);   // prescaler = 4
    TWBR  = 18;                             // 100 kHz at 16 MHz w/ /4 prescaler
    TWCR  = (1 << TWEN);                    // enable TWI (no ISR — TWIE off)
}

bool StartI2C_Trans(unsigned char SLA) {
    // START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    if (!wait_TWINT()) return false;

    // SLA + write
    TWDR = (SLA << 1) | 0;
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!wait_TWINT()) return false;

    // Status 0x18 = MT_SLA_ACK (slave acknowledged the address). Anything
    // else (0x20 NACK, bus error, etc.) → no slave, treat as failure.
    if ((TWSR & 0xF8) != 0x18) {
        StopI2C_Trans();
        return false;
    }
    return true;
}

void StopI2C_Trans() {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    // Wait for the STOP to actually complete on the bus. TWSTO is
    // auto-cleared by hardware once the STOP condition is generated.
    // Without this, back-to-back transactions (the bus scan, or six
    // per-byte accelerometer reads in a row) race the next START
    // against the in-flight STOP and the slave silently NACKs.
    uint16_t timeout = 10000;
    while ((TWCR & (1 << TWSTO)) && --timeout) {
        ;
    }
}

bool Write(unsigned char data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!wait_TWINT()) return false;

    // Status 0x28 = MT_DATA_ACK (slave acknowledged the data byte).
    if ((TWSR & 0xF8) != 0x28) {
        StopI2C_Trans();
        return false;
    }
    return true;
}

// Read a single byte from MEMADDRESS on SLA. Leaves the byte in TWDR for
// Read_data() to fetch. Returns false if the bus hangs at any step.
bool Read_from(unsigned char SLA, unsigned char MEMADDRESS) {
    if (!StartI2C_Trans(SLA)) return false; // START + SLA+W
    if (!Write(MEMADDRESS))   return false; // register pointer

    // Repeated START
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    if (!wait_TWINT()) return false;

    // SLA + read
    TWDR = (SLA << 1) | 1;
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!wait_TWINT()) return false;

    // Status 0x40 = MR_SLA_ACK (slave acknowledged SLA+R).
    if ((TWSR & 0xF8) != 0x40) {
        StopI2C_Trans();
        return false;
    }

    // Clock in the data byte. TWEA cleared so the master NACKs after the
    // single byte we want.
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!wait_TWINT()) return false;

    StopI2C_Trans();
    return true;
}

unsigned char Read_data() {
    return TWDR;
}

unsigned char I2C_status() {
    return TWSR & 0xF8;
}

// Probe the bus for a slave at SLA by issuing START + SLA+W + STOP and
// returning the TWSR status. Useful return codes:
//   0x18 = SLA+W ACKed (slave alive at SLA)
//   0x20 = SLA+W NACKed (no slave at SLA)
//   0xFF = START couldn't be generated (bus stuck — no pull-ups?)
//   0xFE = SLA+W timed out (bus stuck after START)
unsigned char I2C_ping(unsigned char SLA) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    if (!wait_TWINT()) return 0xFF;

    TWDR = (SLA << 1) | 0;
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (!wait_TWINT()) {
        StopI2C_Trans();
        return 0xFE;
    }

    unsigned char status = TWSR & 0xF8;
    StopI2C_Trans();
    return status;
}

// Refresh the six accelerometer byte globals. Returns false if any I²C
// transaction fails (NACK or bus timeout) — in that case we zero the
// globals so callers can't accidentally interpret the stale TWDR junk
// (which is what was making the piezo scream when the bus was dead).
bool read_acceleration() {
    if (!Read_from(MPU6050_ADDR, ACCEL_XOUT_H)) goto fail;
    accelX1 = Read_data();
    if (!Read_from(MPU6050_ADDR, ACCEL_XOUT_L)) goto fail;
    accelX0 = Read_data();
    if (!Read_from(MPU6050_ADDR, ACCEL_YOUT_H)) goto fail;
    accelY1 = Read_data();
    if (!Read_from(MPU6050_ADDR, ACCEL_YOUT_L)) goto fail;
    accelY0 = Read_data();
    if (!Read_from(MPU6050_ADDR, ACCEL_ZOUT_H)) goto fail;
    accelZ1 = Read_data();
    if (!Read_from(MPU6050_ADDR, ACCEL_ZOUT_L)) goto fail;
    accelZ0 = Read_data();
    return true;

fail:
    accelX0 = accelX1 = accelY0 = accelY1 = accelZ0 = accelZ1 = 0;
    return false;
}

bool tilt_exceeded() {
    int16_t y = ((int16_t)(int8_t)accelY1 << 8) | accelY0;
    int16_t z = ((int16_t)(int8_t)accelZ1 << 8) | accelZ0;
    // |Y| above threshold catches Y-axis tilt; |Z| dropping below threshold
    // catches X/Z-axis tilt because gravity rotates out of the +Z direction.
    return (abs(y) > TILT_THRESHOLD) || (abs(z) < TILT_THRESHOLD);
}

bool read_acceleration_threshold() {
    return read_acceleration() && tilt_exceeded();
}
