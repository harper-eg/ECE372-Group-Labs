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

    TCCR3A |= ((1 << WGM31) | (1 << WGM30));
    TCCR3B |= (1 << WGM32) | (1<<CS31);
    TCCR3B &= ~((1 << WGM33) |(1<<CS30)|(1<<CS32));
    

    // TODO: Set OC3A output pin (PE3, Arduino pin 5) direction via DDRE
    DDRE |= (1<<DDE3);
    // TODO: Configure compare output mode (COM3A bits in TCCR3A) will need to decide 
    TCCR3A |= (1<<COM3A1);
    TCCR3A &= ~(1<<COM3A0);
    // TODO: Set prescaler (CS3x bits in TCCR3B)
OCR3A =  255;
}

/*
 * Changes the duty cycle of the PWM signal based on the 10-bit ADC value.
 * The adcValue is the combination of ADCH and ADCL (0-1023).
 */
void changeDutyCycle(unsigned int adcValue){
    int dutyCycle;
    // TODO: Map adcValue to duty cycle and direction
    if (adcValue < 127) {
        dutyCycle = (adcValue - 127) * 255 * 2 / 1023; // Map 0-1023 to 0-255
        PORTB |= (1 << PORTB0); // Clockwise: PB0 = HIGH, PB1 = LOW
        PORTB &= ~(1 << PORTB1);
    }
    else if (adcValue == 127) {
        dutyCycle = 0; // Stop motor at midpoint
        PORTB &= ~((1 << PORTB0) | (1 << PORTB1)); // Stop: PB0 = LOW, PB1 = LOW
    }
    else {
        dutyCycle = adcValue * 255 * 2 / 1023; // Map 0-1023 to 0-255
        PORTB |= (1 << PORTB1); // Counterclockwise: PB0 = LOW, PB1 = HIGH
        PORTB &= ~(1 << PORTB0);
    }

    // D = OCR3A/ TOP
    // TODO: Set OCR3A (and optionally OCR4A) to control speed
    OCR3A = dutyCycle;
    // TODO: Control direction via PB0 (IN1) and PB1 (IN2) on L293D:
    //       Clockwise:          PB0 = HIGH, PB1 = LOW
    //       Counterclockwise:   PB0 = LOW,  PB1 = HIGH
    //       Stop:               PB0 = LOW,  PB1 = LOW

}
