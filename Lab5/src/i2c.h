#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

// ADXL345 register map.
// (The lab spec is written for the MPU-6050; the actual hardware on this
// kit is an ADXL345, which has a different address and register layout.)
#define ACCEL_ADDR    0x53   // SDO/ALT_ADDRESS grounded → 0x53. Use 0x1D if SDO is tied high.
#define DEVID         0x00   // chip-ID register: returns 0xE5 on a real ADXL345
#define POWER_CTL     0x2D   // bit 3 (Measure) must be set for the chip to take measurements
#define DATA_FORMAT   0x31   // sensor range / resolution

// 16-bit acceleration data. ADXL345 stores the LOW byte at the lower
// register address (the opposite of the MPU-6050).
#define ACCEL_XOUT_L  0x32
#define ACCEL_XOUT_H  0x33
#define ACCEL_YOUT_L  0x34
#define ACCEL_YOUT_H  0x35
#define ACCEL_ZOUT_L  0x36
#define ACCEL_ZOUT_H  0x37

// Legacy aliases — earlier MPU-6050 code used these names.
#define MPU6050_ADDR  ACCEL_ADDR
#define PWR_MGMT_1    POWER_CTL

// I2C primitives required by the lab spec. The transaction-level helpers
// return false if the TWINT busy-wait times out (i.e. the bus is hung).
void           InitI2C();
bool           StartI2C_Trans(unsigned char SLA);
void           StopI2C_Trans();
bool           Write(unsigned char data);
bool           Read_from(unsigned char SLA, unsigned char MEMADDRESS);
unsigned char  Read_data();

// Diagnostics helpers.
unsigned char  I2C_status();             // upper 5 bits of TWSR
unsigned char  I2C_ping(unsigned char SLA);  // see i2c.cpp for return codes

// Higher-level accelerometer helpers.
bool read_acceleration();             // refresh accelN0/N1 globals; false on I²C failure
bool tilt_exceeded();                 // threshold check using current globals
bool read_acceleration_threshold();   // legacy: read_acceleration() && tilt_exceeded()

#endif
