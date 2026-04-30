// Authors:
// Net IDs:
// Date:
// Assignment: Lab 5
//
// Push-button used to silence the chirping alarm.
// Default pin: PE4 / INT4 (Arduino D2). PD0 is unavailable because it is
// the I²C SCL line on the Mega2560.
//----------------------------------------------------------------------//

#include "switch.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile buttonState switchState = wait_press;

void initSwitch() {
    DDRE  &= ~(1 << DDE4);          // PE4 input
    PORTE |=  (1 << PORTE4);        // internal pull-up

    // INT4 fires on any logical change so we can debounce both press &
    // release. INT4-INT7 are configured via EICRB (ISC4[1:0] = 01 → any edge).
    EICRB |=  (1 << ISC40);
    EICRB &= ~(1 << ISC41);
    EIMSK |=  (1 << INT4);
}

ISR(INT4_vect) {
    if (switchState == wait_press) {
        switchState = debounce_press;
    } else if (switchState == wait_release) {
        switchState = debounce_release;
    }
}
