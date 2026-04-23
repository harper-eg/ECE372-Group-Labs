#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

void initPWM();
void changeDutyCycle(unsigned int adcValue);

// TODO: Add any additional PWM function prototypes as needed

#endif