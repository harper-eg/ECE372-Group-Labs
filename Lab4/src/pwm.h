// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: Function prototypes for pwm.cpp (PWM signal generation)
//----------------------------------------------------------------------//

#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

void initPWM();
void changeDutyCycle(unsigned int adcValue);

// TODO: Add any additional PWM function prototypes as needed

#endif
