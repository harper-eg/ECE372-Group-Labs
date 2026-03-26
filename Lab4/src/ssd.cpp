// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: This file implements the seven segment display
// driven via shift register on PORTC pins (PC0-PC2) in common cathode
// configuration.
//
// Pin mapping (from mappings.txt):
//   PC0 -> Shift register Q1 (e.g., SER / serial data)
//   PC1 -> Shift register Q2 (e.g., SRCLK / shift clock)
//   PC2 -> Shift register Q3 (e.g., RCLK / latch clock)
//----------------------------------------------------------------------//

#include "ssd.h"

/*
 * Initializes PORTC pins as outputs for the seven segment display.
 */
void initSSD(){
    // Set PC0, PC1, PC2 as outputs for shift register control
    DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2);
}

/*
 * Displays a single digit (0-9) on the seven segment display.
 * Common cathode configuration.
 */
void displayDigit(unsigned char digit){
    // TODO: Create lookup table for segment patterns (common cathode, 0-9)
    // TODO: Serially shift the segment pattern out via PC0 (data),
    //       PC1 (shift clock), PC2 (latch clock)
}

/*
 * Turns off all segments of the seven segment display.
 */
void turnOffSSD(){
    // TODO: Shift out all zeros to turn off display segments
}
