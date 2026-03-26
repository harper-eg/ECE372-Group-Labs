// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: This file describes functions used by the timer
//----------------------------------------------------------------------//

#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

void initTimer0();
void delayMs(unsigned int delay);
void initTimer1();

// TODO: Add any additional timer function prototypes needed for
// the 10-second countdown (Timer1 with ISR)

#endif
