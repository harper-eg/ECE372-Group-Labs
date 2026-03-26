// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: This file implements ADC initialization and reading
// for the potentiometer on pin A0.
//----------------------------------------------------------------------//

#include "adc.h"

/*
 * Initializes the ADC. Uses A0 pin as input.
 * On ATmega2560, Arduino A0 = PF0 (ADC0).
 */
void initADC(){
    // A0 (PF0) as input — clear direction bit
    DDRF &= ~(1 << DDF0);

    // TODO: Set reference voltage (REFS1:REFS0 in ADMUX)
    // TODO: Select ADC channel 0 (MUX4:0 = 00000 in ADMUX/ADCSRB) for PF0/ADC0
    // TODO: Enable the ADC (ADEN in ADCSRA)
    // TODO: Set ADC prescaler for appropriate clock division (ADPS bits in ADCSRA)
}

// TODO: Implement function to start conversion and read 10-bit ADC result
// (combine ADCL and ADCH registers)
