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

    ADMUX |= (1 << REFS0); // Set reference voltage to AVCC (0.1V - 5.0V)
    ADMUX &= ~(1 << REFS1); // Clear REFS1 for AVCC reference
    ADMUX &= ~(1 << ADLAR); // Clear ADLAR for right-adjusted result
    ADMUX &= ~(0x1F); // Clear MUX4:0 bits to select ADC0 (PF0)
    ADCSRA |= (1 << ADEN); // Enable the ADC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set prescaler to 128 for 125kHz ADC clock (16MHz / 128)
    
    // TODO: Set reference voltage (REFS1:REFS0 in ADMUX)
    // TODO: Select ADC channel 0 (MUX4:0 = 00000 in ADMUX/ADCSRB) for PF0/ADC0
    // TODO: Enable the ADC (ADEN bit in ADCSRA)
    // TODO: Set ADC prescaler for appropriate clock division (ADPS bits in ADCSRA)
}

int readADC() {
    // Start conversion by setting ADSC bit in ADCSRA
    ADCSRA |= (1 << ADSC);
    
    // Wait for conversion to complete (ADSC becomes 0 when done)
    while (ADCSRA & (1 << ADSC));
    
    // Read the 10-bit ADC result from ADCL and ADCH
    int adcValue = ADCL; // Read lower byte first
    adcValue |= (ADCH << 8); // Read upper byte and combine
    
    return adcValue;
}

// TODO: Implement function to start conversion and read 10-bit ADC result
// (combine ADCL and ADCH registers)
