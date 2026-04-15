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

}