// Authors:
// Net IDs:
// Date:
// Assignment: Lab 5
//
// Description: 
//----------------------------------------------------------------------//

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "switch.h"
#include "timer.h"
#include "pwm.h"
#include "i2c.h"
#include "spi.h"

// --- MPU-6050 register map (Register Map datasheet, I2C SLA = WHO_AM_I = 0x68) ---
#define MPU6050_ADDR  0x68   // 7-bit slave address (WHO_AM_I value, pg. 46)
#define PWR_MGMT_1    0x6B   // Power management register (pg. 41); write 0x00 to wake
#define ACCEL_XOUT_H  0x3B   // X-axis high byte (pg. 30)
#define ACCEL_XOUT_L  0x3C   // X-axis low byte
#define ACCEL_YOUT_H  0x3D   // Y-axis high byte
#define ACCEL_YOUT_L  0x3E   // Y-axis low byte
#define ACCEL_ZOUT_H  0x3F   // Z-axis high byte
#define ACCEL_ZOUT_L  0x40   // Z-axis low byte

// Tilt threshold in raw LSB units.
// At ±2g full scale: 1g = 16384 LSB; sin(45°)*16384 ≈ 11585.
// TUNE EXPERIMENTALLY using Serial output before finalising.
#define ACCEL_THRESHOLD 11000

// OCR3A value for ~50% duty cycle on the piezo.
// Timer3 Fast PWM 10-bit (TOP=1023), prescaler 8 → f_PWM ≈ 1953 Hz (audible).
#define CHIRP_OCR  512

// write_LED is implemented in spi.cpp; 1 = smiley face, 0 = frowny face.
extern void write_LED(bool face);

// --- State machine type definitions ---

typedef enum buttonState_enum {
  wait_press, debounce_press,
  wait_release, debounce_release
} buttonState;

typedef enum appState_enum {
  alarm_off,   // below threshold: smiley face displayed, no alarm
  alarm_on     // threshold exceeded: frowny face + chirping piezo alarm
} appState_t;

// --- Volatile shared state (accessed from ISRs) ---

volatile buttonState switchState = wait_press;
volatile appState_t  appState    = alarm_off;
volatile bool        chirpToggle = false;  // toggled by Timer1 ISR

int main() {

  sei();           // enable global interrupts (required for Serial.println)

  initTimer0();    // Timer0: millisecond delay used for switch debouncing
  initTimer1();    // Timer1: CTC mode; reconfigured here for chirp intervals
  initSwitchPD0(); // PD0 as input with pull-up resistor
  initPWM();       // Timer3 PWM on PE3 (Arduino pin 5) for piezo buzzer
  initI2C();       // I2C (TWI) for MPU-6050 accelerometer
  initSPI();       // SPI for 8x8 MAX7219 LED matrix
  Serial.begin(9600);

  // initSwitchPD0 (Lab 5 version) does not configure INT0; do it here.
  EICRA |= (1 << ISC00);    // any logical change on INT0
  EICRA &= ~(1 << ISC01);
  EIMSK |= (1 << INT0);     // enable INT0

  // Wake up MPU-6050: write 0x00 to PWR_MGMT_1 (clears sleep bit)
  sendDataI2C(MPU6050_ADDR, PWR_MGMT_1, 0x00);

  // Display smiley face at startup (below threshold assumed)
  write_LED(1);

  while (1) {

    // ----------------------------------------------------------------
    // Switch debounce state machine
    // ----------------------------------------------------------------
    switch (switchState) {
      case wait_press:
        break;

      case debounce_press:
        delayMs(1);
        switchState = wait_release;
        break;

      case wait_release:
        break;

      case debounce_release:
        delayMs(1);
        switchState = wait_press;
        // Button fully released: silence the alarm if it is active
        if (appState == alarm_on) {
          appState = alarm_off;
          OCR3A = 0;                                    // stop piezo tone
          TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));  // stop Timer1
          TIMSK1 &= ~(1 << OCIE1A);                    // disable Timer1 IRQ
          write_LED(1);                                 // restore smiley face
        }
        break;
    }

    // ----------------------------------------------------------------
    // Read accelerometer: X, Y, Z high and low bytes over I2C
    //
    // NOTE: The current i2c.cpp read() issues a repeated-start and reads
    // two bytes (MPU-6050 auto-increments the register pointer).  Only the
    // last byte is accessible via read_data() / TWDR.  If readings look
    // wrong, update i2c.cpp to perform a single-byte read per call.
    // ----------------------------------------------------------------

    // X axis
    read(MPU6050_ADDR, ACCEL_XOUT_H);
    int8_t xH = (int8_t)read_data();
    read(MPU6050_ADDR, ACCEL_XOUT_L);
    uint8_t xL = read_data();

    // Y axis
    read(MPU6050_ADDR, ACCEL_YOUT_H);
    int8_t yH = (int8_t)read_data();
    read(MPU6050_ADDR, ACCEL_YOUT_L);
    uint8_t yL = read_data();

    // Z axis
    read(MPU6050_ADDR, ACCEL_ZOUT_H);
    int8_t zH = (int8_t)read_data();
    read(MPU6050_ADDR, ACCEL_ZOUT_L);
    uint8_t zL = read_data();

    // Combine high and low bytes into signed 16-bit values
    int16_t accelX = ((int16_t)xH << 8) | xL;
    int16_t accelY = ((int16_t)yH << 8) | yL;
    int16_t accelZ = ((int16_t)zH << 8) | zL;

    // Print for debugging and threshold calibration
    Serial.print("X: "); Serial.print(accelX);
    Serial.print("  Y: "); Serial.print(accelY);
    Serial.print("  Z: "); Serial.println(accelZ);

    // ----------------------------------------------------------------
    // Alarm / display state machine
    // ----------------------------------------------------------------
    switch (appState) {

      case alarm_off:
        // Check for tilt on Y or Z axis exceeding ~45 degrees.
        // At rest: Y ≈ 0, Z ≈ 16384.  At 45° tilt: |Y| or |Z| ≈ 11585.
        // Z check: abs(accelZ) < ACCEL_THRESHOLD catches Z dropping from 16384.
        if (abs(accelY) > ACCEL_THRESHOLD || abs(accelZ) < ACCEL_THRESHOLD) {
          appState = alarm_on;
          write_LED(0);   // switch to frowny face

          // Configure Timer1 for 100ms chirp-toggle intervals:
          // 16 MHz / 1024 prescaler = 15625 Hz; 100ms → OCR1A = 1561
          OCR1A  = 1561;
          TCNT1  = 0;
          TIMSK1 |= (1 << OCIE1A);
          TCCR1B |= (1 << CS12) | (1 << CS10);
          TCCR1B &= ~(1 << CS11);
        }
        break;

      case alarm_on:
        // Piezo chirps by toggling OCR3A between tone and silence.
        // chirpToggle is flipped every 100ms by the Timer1 ISR.
        OCR3A = chirpToggle ? CHIRP_OCR : 0;
        break;
    }

  }

  return 0;
}

// INT0 ISR 
ISR(INT0_vect) {
  if (switchState == wait_press) {
    switchState = debounce_press;
  } else if (switchState == wait_release) {
    switchState = debounce_release;
  }
}

// Timer1 compare-match ISR — fires every 100ms to toggle the piezo chirp
ISR(TIMER1_COMPA_vect) {
  chirpToggle = !chirpToggle;
}
