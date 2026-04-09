// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: This file implements the seven segment display
// driven via shift register on PORTC pins (PC0-PC2) in common cathode
// configuration.
//----------------------------------------------------------------------//

#include "ssd.h"
#include "timer.h"

// Initialize PORTC pins as outputs for the seven segment display
void initSSD(){
    // Set PC0, PC1, PC2 as outputs for shift register control
    // Map PC0 = SER, PC1 = SRCLK, PC2 = RCLK
    DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2);
}

// Display a single digit (0-9) on the seven segment display.
void displayDigit(unsigned char digit){
    // Segment patterns for common cathode display
    unsigned char lookup[] = {
        0b00111111, // 0: a,b,c,d,e,f
        0b00000110, // 1: b,c
        0b01011011, // 2: a,b,d,e,g
        0b01001111, // 3: a,b,c,d,g
        0b01100110, // 4: b,c,f,g
        0b01101101, // 5: a,c,d,f,g
        0b01111101, // 6: a,c,d,e,f,g
        0b00000111, // 7: a,b,c
        0b01111111, // 8: a,b,c,d,e,f,g
        0b01101111  // 9: a,b,c,d,f,g
    };

    unsigned char data = lookup[digit];

    // Pull latch (PC2) low before shifting data
    PORTC &= ~(1 << PORTC2);

    // Shift out 8 bits
    for (int i = 7; i >= 0; i--) {
        // Set data bit on PC0
        if (data & (1 << i)) {
            PORTC |= (1 << PORTC0);
        } else {
            PORTC &= ~(1 << PORTC0);
        }

        // Pulse shift clock (PC1)
        PORTC |= (1 << PORTC1);
        delayMs(1); // Short delay to ensure clock is registered
        PORTC &= ~(1 << PORTC1);
    }

    // Pulse latch clock (PC2) to output data to display
    PORTC |= (1 << PORTC2);
    delayMs(1); // Short delay to ensure data is latched
    PORTC &= ~(1 << PORTC2);
}

/*
 * Turns off all segments of the seven segment display.
 */
void turnOffSSD(){
    PORTC &= ~(1 << PORTC2);

    // Want to shift out 8 zeros, so PC0 stays 0
    PORTC &= ~(1 << PORTC0);

    // Repeatedly pulse shift register clock to input data
    for (int i = 0; i < 8; i++) {
        PORTC |= (1 << PORTC1);
        delayMs(1); // Short delay to ensure clock is registered
        PORTC &= ~(1 << PORTC1);
    }

    // Pulse latch to output
    PORTC |= (1 << PORTC2);
    delayMs(1); // Short delay to ensure data is latched
    PORTC &= ~(1 << PORTC2);
}
