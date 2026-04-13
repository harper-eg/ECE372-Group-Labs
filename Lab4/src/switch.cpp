// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: This file implements the initialization of an external
// switch using INT0 on PORTD0.
//----------------------------------------------------------------------//

#include "switch.h"
#include <avr/io.h>

/*
 * Initializes pull-up resistor on PD0 and sets it into input mode.
 * Configures external interrupt INT0 for any logical change.
 */
void initSwitchPD0(){
    // PD0 as input (similar pattern to Lab 2/3 initSwitchPB3)
    DDRD &= ~(1 << DDD0); // Input
    PORTD |= (1 << PORTD0); // Enable pull-up

    // Configure INT0 for any logical change
    EICRA |= (1 << ISC00);
    EICRA &= ~(1 << ISC01);

    // Enable INT0
    EIMSK |= (1 << INT0);
}

void disableINT0() {
    EIMSK &= ~(1 << INT0);
}

void enableINT0() {
    EIMSK |= (1 << INT0);
}
