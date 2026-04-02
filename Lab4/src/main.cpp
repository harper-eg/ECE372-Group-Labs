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


/*
 * Define a set of states that can be used in the state machine using an enum.
 * (Pattern reused from Lab 2/3)
 */
typedef enum buttonState_enum {
  wait_press, debounce_press,
  wait_release, debounce_release
} buttonState;

// TODO: Define states for the main state machine
// e.g., motor_running, motor_off_countdown, etc.

// Initialize states
volatile buttonState switchState = wait_press;
// TODO: Add volatile variables for countdown, motor state, etc.


int main(){

  sei(); // Enable global interrupts (from Lab 2/3)

  initTimer0();
  initTimer1();
  initSwitchPD0();
  initADC();
  initPWM();
  initSSD();

  // TODO: Initialize main state machine state


  /*
  * Implement a state machine in the while loop which achieves the assignment
  * requirements. (Pattern reused from Lab 2/3)
  */
	while (1) {
    // Debouncing for button (reused from Lab 2/3)
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

	}

  return 0;
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
