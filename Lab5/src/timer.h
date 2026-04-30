// Authors:
// Net IDs:
// Date:
// Assignment: Lab 5
//
// Description: Millisecond timer used for switch debouncing (Timer 1).
//----------------------------------------------------------------------//

#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

void initTimer1();
void delayMs(unsigned int delay);

#endif
