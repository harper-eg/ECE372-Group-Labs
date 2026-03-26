// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: This file implements PWM signal generation on Timer 3
// (and optionally Timer 4) to control DC motor speed and direction
// via the L293D driver.
//----------------------------------------------------------------------//

#include "pwm.h"

/*
 * Initializes PWM output on Timer 3 (and optionally Timer 4).
 * Set appropriate PWM mode and output pin(s).
 */
void initPWM(){
    // Motor direction pins: PB0 (IN1) and PB1 (IN2) on L293D
    DDRB |= (1 << DDB0) | (1 << DDB1); // Set PB0 and PB1 as outputs

    // TODO: Configure Timer3 for PWM mode (Fast PWM or Phase Correct)
    //       using WGM bits in TCCR3A/TCCR3B
    // TODO: Set OC3A output pin (PE3, Arduino pin 5) direction via DDRE
    // TODO: Configure compare output mode (COM3A bits in TCCR3A)
    // TODO: Set prescaler (CS3x bits in TCCR3B)
    // TODO: (Optional) Configure Timer4 similarly if using two PWM signals
    //       OC4A is on PH3 (Arduino pin 6)
}

/*
 * Changes the duty cycle of the PWM signal based on the 10-bit ADC value.
 * The adcValue is the combination of ADCH and ADCL (0-1023).
 * This value determines both speed and direction per the potentiometer table:
 *   0V (0)       -> clockwise, max rpm
 *   0V-2.5V (0-511)   -> clockwise, decreasing rpm (linearly)
 *   2.5V (512)   -> stopped (0 rpm)
 *   2.5V-5V (512-1023) -> counterclockwise, increasing rpm (linearly)
 *   5V (1023)    -> counterclockwise, max rpm
 */
void changeDutyCycle(unsigned int adcValue){
    // TODO: Map adcValue to duty cycle and direction
    // TODO: Set OCR3A (and optionally OCR4A) to control speed
    // TODO: Control direction via PB0 (IN1) and PB1 (IN2) on L293D:
    //       Clockwise:          PB0 = HIGH, PB1 = LOW
    //       Counterclockwise:   PB0 = LOW,  PB1 = HIGH
    //       Stop:               PB0 = LOW,  PB1 = LOW
}
