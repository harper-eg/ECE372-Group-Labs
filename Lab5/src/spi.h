#ifndef SPI_H

#define SPI_H

#include <avr/io.h>

// put functions for spi.cpp
void initSPI();
void writeSPI(unsigned char regAddress, int data);
unsigned char readSPI( unsigned char regAddress);
int* readAccel();
void writeLED(bool faceState);


#endif
