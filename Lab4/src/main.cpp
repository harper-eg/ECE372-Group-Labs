// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
//----------------------------------------------------------------------//

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "switch.h"
#include "timer.h"
#include "adc.h"
#include "pwm.h"
#include "ssd.h"

// Debounce states
typedef enum buttonState_enum {
  wait_press, debounce_press,
  wait_release, debounce_release
} buttonState;

// Main application states
typedef enum mainState_enum {
  motor_running,
  motor_off_countdown
} mainState;

// Initialize states
volatile buttonState switchState = wait_press;
volatile mainState appState = motor_running;

volatile int countdown = 9;
volatile bool sevenSegmentTimerFlag = false;

int main(){

  sei(); // Enable global interrupts

  initTimer0();
  initTimer1();
  initSwitchPD0();
  initADC();
  initPWM();
  initSSD();
  Serial.begin(9600); // Initialize serial communication for debugging

  while (1) {
    // Debouncing for button
    switch(switchState) {
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
        break;
    }

    // Main state machine
    switch(appState) {
      case motor_running:
        // Read potentiometer and drive motor
        {
          int adcValue = readADC();
          changeDutyCycle(adcValue);
        }
        break;

      case motor_off_countdown:
        // Motor stays off
        changeDutyCycle(512); // midpoint = 0 speed

        // Update seven segment display on each timer tick
        if (sevenSegmentTimerFlag) {
          sevenSegmentTimerFlag = false;

          if (countdown >= 0) {
            displayDigit(countdown);
            countdown--;
          }

          if (countdown < 0) {
            // Countdown finished: turn off display, re-enable button, resume motor
            turnOffSSD();
            stopTimer1();
            enableINT0();
            appState = motor_running;
          }
        }
        break;
    }
    
  }

  return 0;
}

// INT0 ISR — handles switch press on PD0 with debounce state machine
ISR(INT0_vect){
  if (switchState == wait_press) {
    switchState = debounce_press;
  }
  else if (switchState == wait_release) {
    // Button released: start countdown, turn off motor
    switchState = debounce_release;

    if (appState == motor_running) {
      appState = motor_off_countdown;
      countdown = 9;
      sevenSegmentTimerFlag = true; // Display 9 immediately
      disableINT0();               // Disable button during countdown
      startTimer1();               // Start 1-second tick
    }
  }
}

// Timer1 compare match ISR — fires every 1 second during countdown
ISR(TIMER1_COMPA_vect) {
  sevenSegmentTimerFlag = true;
}
