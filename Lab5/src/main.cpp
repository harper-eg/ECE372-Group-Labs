// Authors:
// Net IDs:
// Date:
// Assignment: Lab 5
//
// MPU-6050 tilt detector with smiley/frowny LED matrix and chirping piezo
// alarm silenced by a push-button switch.
//
// Two state machines run in main():
//   1. switchState  — debounces the alarm-silence button (driven by INT4)
//   2. appState     — toggles the 8x8 LED face and the chirping alarm based
//                     on the accelerometer threshold and the silence button
//
// All special-function-register access lives inside the driver modules
// (switch.cpp, timer.cpp, pwm.cpp, i2c.cpp, spi.cpp). main() only uses
// their high-level APIs.
//----------------------------------------------------------------------//

#include <Arduino.h>
#include <avr/interrupt.h>
#include "switch.h"
#include "timer.h"
#include "pwm.h"
#include "i2c.h"
#include "spi.h"

// Latest raw accelerometer bytes, owned by i2c.cpp.
extern unsigned char accelX0, accelX1, accelY0, accelY1, accelZ0, accelZ1;

// Display / alarm state machine.
typedef enum appState_enum {
    alarm_off,      // below threshold: smiley face, piezo silent
    alarm_on        // threshold exceeded: frowny face, chirping piezo
} appState_t;

volatile appState_t appState = alarm_off;

// ADXL345's DEVID register returns 0xE5 on a real part. (Kept under the
// WHO_AM_I name so the diagnostic vocabulary still matches the lab spec.)
#define WHO_AM_I_REG    DEVID
#define WHO_AM_I_EXPECT 0xE5

static void printHex2(unsigned char v) {
    if (v < 0x10) Serial.print('0');
    Serial.print(v, HEX);
}

// Walk addresses 0x03..0x77 and print every device that ACKs SLA+W.
// Useful when the chip you have isn't actually at the address you expect
// (e.g. ADXL345 is at 0x53, BMP280 at 0x76, MPU-6050 at 0x68 or 0x69).
static void scanI2CBus() {
    Serial.println(F("I2C scan:"));
    unsigned char found = 0;
    for (unsigned char addr = 0x03; addr <= 0x77; addr++) {
        unsigned char st = I2C_ping(addr);
        if (st == 0x18) {                       // SLA+W ACKed
            Serial.print(F("  device at 0x"));
            printHex2(addr);
            Serial.println();
            found++;
        } else if (st == 0xFF || st == 0xFE) {
            Serial.print(F("  bus timeout at 0x"));
            printHex2(addr);
            Serial.println(F("  — aborting scan"));
            return;                             // bus is dead; stop probing
        }
    }
    if (found == 0) {
        Serial.println(F("  (no devices responded)"));
    }
}

int main() {
    initTimer1();        // Timer1 1 ms tick for debounce delays
    initSwitch();        // PE4 + INT4 for the silence button
    initPWM();           // Timer4 OC4B (PH4) tone, Timer3 chirp ISR
    InitI2C();           // TWI for the MPU-6050
    initSPI();           // SPI + MAX7219 init for the 8x8 LED matrix
    Serial.begin(9600);
    sei();               // required so Serial.print works

    // Show the smiley as soon as SPI is alive, before any I²C activity,
    // so the LED matrix is visibly working even if the bus diagnostics
    // below take time or fail outright.
    write_LED(true);

    // -------------------------------------------------------------------
    // Boot diagnostics
    // -------------------------------------------------------------------
    Serial.println();
    Serial.println(F("=== Lab 5 boot ==="));

    // Direct read of the idle line states. With the TWI peripheral
    // enabled and no transaction running, both SDA and SCL should be
    // released to the bus pull-ups and read as 1. A 0 here means that
    // line is being held low — either a wire shorted to GND, a
    // permanently-driving slave, or the wire isn't actually connected
    // to PD0/PD1 at all (and we're reading a floating, GND-leaning pin).
    unsigned char d = PIND;
    Serial.print(F("Idle bus state: SCL(PD0)="));
    Serial.print((d >> 0) & 1);
    Serial.print(F("  SDA(PD1)="));
    Serial.println((d >> 1) & 1);

    scanI2CBus();

    unsigned char ping = I2C_ping(MPU6050_ADDR);
    Serial.print(F("ping 0x"));
    printHex2(MPU6050_ADDR);
    Serial.print(F(" -> status 0x"));
    printHex2(ping);
    if      (ping == 0x18) Serial.println(F("   ACK (slave alive)"));
    else if (ping == 0x20) Serial.println(F("   NACK (slave not at this address)"));
    else if (ping == 0xFF) Serial.println(F("   START timeout (bus stuck — pull-ups?)"));
    else if (ping == 0xFE) Serial.println(F("   SLA+W timeout (bus stuck after START)"));
    else                   Serial.println(F("   unexpected status"));

    bool ok = Read_from(ACCEL_ADDR, WHO_AM_I_REG);
    unsigned char who = Read_data();
    Serial.print(F("DEVID read_ok="));
    Serial.print(ok ? F("yes") : F("no "));
    Serial.print(F(" value=0x"));
    printHex2(who);
    Serial.println(who == WHO_AM_I_EXPECT ? F("   (matches ADXL345)") : F("   (expected 0xE5)"));

    // ADXL345 default DATA_FORMAT (0x00 = right-justified, ±2 g, 10-bit) is
    // what we want; explicitly write it to be sure, then put the chip into
    // measurement mode by setting POWER_CTL.D3.
    StartI2C_Trans(ACCEL_ADDR);
    Write(DATA_FORMAT);
    Write(0x00);
    StopI2C_Trans();

    StartI2C_Trans(ACCEL_ADDR);
    Write(POWER_CTL);
    Write(0x08);
    StopI2C_Trans();

    Read_from(ACCEL_ADDR, POWER_CTL);
    unsigned char pwr = Read_data();
    Serial.print(F("POWER_CTL = 0x"));
    printHex2(pwr);
    Serial.println(pwr == 0x08 ? F("   (measuring)") : F("   *** measure bit not set ***"));

    Serial.println(F("Entering main loop\n"));

    // Throttle the diagnostic print with a plain iteration counter.
    // millis() would be the obvious choice, but it requires Arduino's
    // init() to start Timer0 — and a freestanding `int main()` never
    // calls init(), so millis() is stuck at 0 here.
    unsigned long iter           = 0;
    uint8_t       print_throttle = 0;
    const uint8_t PRINT_EVERY    = 5;

    while (1) {

        // -----------------------------------------------------------------
        // Switch debounce state machine
        // -----------------------------------------------------------------
        switch (switchState) {
            case wait_press:
                break;

            case debounce_press:
                delayMs(20);
                switchState = wait_release;
                break;

            case wait_release:
                break;

            case debounce_release:
                delayMs(20);
                switchState = wait_press;
                if (appState == alarm_on) {
                    appState = alarm_off;
                    stopChirp();
                    write_LED(true);
                }
                break;
        }

        // -----------------------------------------------------------------
        // Read accelerometer and check threshold. tilted is forced false
        // when reads_ok is false so a hung / unwired bus doesn't latch
        // the alarm on garbage TWDR data.
        // -----------------------------------------------------------------
        bool reads_ok = read_acceleration();
        bool tilted   = reads_ok && tilt_exceeded();

        // -----------------------------------------------------------------
        // Alarm / display state machine
        // -----------------------------------------------------------------
        switch (appState) {
            case alarm_off:
                if (tilted) {
                    appState = alarm_on;
                    write_LED(false);
                    startChirp();
                }
                break;

            case alarm_on:
                // Chirp runs in the background via Timer4 + Timer3 ISR.
                // Stays on until the button silences it.
                break;
        }

        iter++;

        // -----------------------------------------------------------------
        // Throttled diagnostic print — every PRINT_EVERY-th loop iteration.
        // Serial output further self-throttles at 9600 baud so the rate
        // ends up around 2-3 Hz, which is comfortably readable.
        // -----------------------------------------------------------------
        if (++print_throttle >= PRINT_EVERY) {
            print_throttle = 0;

            int16_t accelX = ((int16_t)(int8_t)accelX1 << 8) | accelX0;
            int16_t accelY = ((int16_t)(int8_t)accelY1 << 8) | accelY0;
            int16_t accelZ = ((int16_t)(int8_t)accelZ1 << 8) | accelZ0;

            Serial.print(F("iter="));   Serial.print(iter);
            Serial.print(F("  i2c="));   Serial.print(reads_ok ? F("ok  ") : F("FAIL"));
            Serial.print(F("  X="));    Serial.print(accelX);
            Serial.print(F("  Y="));    Serial.print(accelY);
            Serial.print(F("  Z="));    Serial.print(accelZ);
            Serial.print(F("  raw="));
            printHex2(accelX1); printHex2(accelX0); Serial.print(' ');
            printHex2(accelY1); printHex2(accelY0); Serial.print(' ');
            printHex2(accelZ1); printHex2(accelZ0);
            Serial.print(F("  tilt="));  Serial.print(tilted ? F("YES") : F("no "));
            Serial.print(F("  state=")); Serial.println(appState == alarm_on ? F("ALARM") : F("off"));
        }
    }

    return 0;
}
