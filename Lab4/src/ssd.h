// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: Function prototypes for ssd.cpp (Seven Segment Display)
//----------------------------------------------------------------------//

#ifndef SSD_H
#define SSD_H

#include <avr/io.h>

void initSSD();
void displayDigit(unsigned char digit);
void turnOffSSD();

#endif
