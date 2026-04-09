// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
//----------------------------------------------------------------------//

//taysom 

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "switch.h"
#include "timer.h"
#include "adc.h"
#include "pwm.h"
#include "ssd.h"


// Define a set of states that can be used in the state machine using an enum.
typedef enum buttonState_enum {
  wait_press, debounce_press,
  wait_release, debounce_release
} buttonState;

// TODO: Define states for the main state machine
// e.g., motor_running, motor_off_countdown, etc.

// Initialize states
volatile buttonState switchState = wait_press;
// TODO: Add volatile variables for countdown, motor state, etc.

volatile int countdown = 9; // Example variable for countdown, adjust as needed

volatile bool sevenSegmentTimerFlag = true; // Flag to indicate when to update seven segment display

int main(){

  sei(); // Enable global interrupts

  initTimer0();
  initTimer1();
  initSwitchPD0();
  initADC();
  initPWM();
  initSSD();

  // TODO: Initialize main state machine state


  /*
  * Implement a state machine in the while loop which achieves the assignment
  * requirements. 
  */
	while (1) {
    // Debouncing for button 
    switch(switchState) {
      case wait_press:
        break;
      case debounce_press:
        delayMs(1);
        countdown = 9; // Reset countdown when button is pressed
        switchState = wait_release;
        break;
      case wait_release:
        break;
      case debounce_release:
        delayMs(1);
        switchState = wait_press;
        break;
    }
    
    // TODO: Implement main state machine
    // State: motor_running
    //   - Read ADC value from potentiometer

    //   - Call changeDutyCycle() with the ADC value
    //   - Motor runs at speed/direction per potentiometer table
    //
    // State: motor_off_countdown
    //   - Motor is off
    //   - Seven segment display counts down from 9 to 0
    //   - INT0 interrupt is disabled
    //   - When countdown reaches 0, turn off display, re-enable INT0,
    //     and transition back to motor_running
    if (switchState == wait_release && countdown <= 0) {
      countdown = 9; // Reset countdown when button is pressed
      startTimer1(); // Start timer 1 for countdown
      // start timer 1 for countdown
    }

    if (countdown > 0) {
      int adcValue = readADC();
      changeDutyCycle(adcValue);
    }
    else {
      changeDutyCycle(0); // Turn off motor when countdown reaches 0
    }
  
    if (sevenSegmentTimerFlag) {
      if (countdown >= 0) {
      displayDigit(countdown);
      countdown -= 1; // Example countdown decrement, replace with actual timer interrupt logic
      }
      else {
        turnOffSSD();
      }
      
      if (countdown >= 0) {
        displayDigit(countdown);
        countdown -= 1; // Example countdown decrement, replace with actual timer interrupt logic
      }
      else {
        turnOffSSD();
      }
      sevenSegmentTimerFlag = false; // Reset flag until next timer interrupt
      startTimer1(); // Start timer 1 for countdown
    }
  return 0;
  }
}

/* INT0 ISR - Handles the switch press on PORTD0.
*  Uses debounce state machine pattern from Lab 2/3.
*  On valid press: turn motor off, start 9-to-0 countdown.
*/
ISR(INT0_vect){
  // Debounce 
  if (switchState == wait_press) {
    switchState = debounce_press;
  }
  else if (switchState == wait_release) {
    // TODO: Set state to begin countdown / turn off motor
    // TODO: Disable INT0 during countdown

    switchState = debounce_release;
  }
}

// TODO: ISR for Timer1 compare match (TIMER1_COMPA_vect)
// - Decrement countdown variable
// - Update seven segment display
// - When countdown reaches 0: turn off display, stop Timer1,
//   re-enable INT0, transition state back to motor_running

ISR(TIMER1_COMPA_vect) {
  if (countdown > 0) {
    sevenSegmentTimerFlag = true; // Set flag to update seven segment display in main loop
  }
}