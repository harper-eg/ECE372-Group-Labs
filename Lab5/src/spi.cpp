
#include <Arduino.h>
#include "spi.h"
#include <avr/io.h>
#include <stdio.h>

#define wait_for_completion while(!(SPSR & (1<<SPIF)));
unsigned char DATAX0_REG = 0x32; // X-Axis Data 0 Register
unsigned char DATAX1_REG = 0x33; // X-Axis Data 1 Register
unsigned char DATAY0_REG = 0x34; // Y-Axis Data 0 Register
unsigned char DATAY1_REG = 0x35; // Y-Axis Data 1 Register
unsigned char DATAZ0_REG = 0x36; // Z-Axis Data 0 Register
unsigned char DATAZ1_REG = 0x37; // Z-Axis Data 1 Register
unsigned char dataX0, dataX1, dataY0, dataY1, dataZ0, dataZ1;
int dataX, dataY, dataZ;

void initSPI() {
    // set the SS, MOSI, and SCLK pin as output
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2);
    // pull up resistors
    PINB |= (1 << PINB0) | (1 << PINB1) | (1 << PINB2);
    // set the MISO pin as input
    DDRB &= ~(1 << DDB3);

    // set SS high at first
    PORTB |= (1 << PORTB0);
    // enable the interrupt, SPI, master mode
    SPCR |= (1 << SPE)|(1 << MSTR);
    //CPOL, CPHA, default clock, and fosc/128
    //ADXL345 (Accelormeter) CPOL = 1, CPHA = 1
    SPCR |=(1 << CPOL)|(1 << CPHA)|(1 << SPR1)|(1 << SPR0);
    // set DORD to MSB first
    SPCR &= ~(1 <<DORD);
    // disable the SPI interrupt (not using ISRs)  
    SPCR &= ~(1 << SPIE);

    //ADXL345 CPOL = 1, CPHA = 1
    SPCR |= (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (1 << CPOL)| (1 << CPHA)| (1 << SPR1) | (1 << SPR0);

    // Initialize data read variables
    dataX0 = 0;
    dataX1 = 0;
    dataX = 0;
    dataY0 = 0;
    dataY1 = 0;
    dataY = 0;
    dataZ0 = 0;
    dataZ1 = 0;
    dataZ = 0;


}

void writeSPI(unsigned char regAddress, int data) {
    // set SS low to begin SPI frame
    PORTB &= ~(1 << PORTB0);
    // register address with write bit and the internal register address
    SPDR = 0x00 | regAddress;
    //wait_for_completion
    while(!(SPSR & (1<<SPIF)));
    SPDR = data;
    //wait_for_completion
    while(!(SPSR & (1<<SPIF)));
    // set SS high to end SPI frame
    PORTB |= (1 << PORTB0);

}

unsigned char readSPI( unsigned char regAddress) {
    // set SS low to begin SPI frame
    PORTB &= ~(1 << PORTB0);
    // register address with read bit and the internal register address
    SPDR = 0x80 | regAddress;
    wait_for_completion;
    //wait_for_completion
    while(!(SPSR & (1<<SPIF)));
    // read the received data
    unsigned char receivedData = SPDR;
    // set SS high to end SPI frame
    PORTB |= (1 << PORTB0);
    return receivedData;
}
// 1.All communication with the 8 x8 led matrix must be done over the SPI
// related pins.

// 2.Read the 8x8 MAX7219 datasheet for setting up SPI mode.

//read accel and print
int* readAccel(){
    // Read X0 & X1 Data
    dataX0 = readSPI(DATAX0_REG);
    dataX1 = readSPI(DATAX1_REG);
    // Combine X0 and X1
    dataX = (dataX1 << 8) | dataX0;
    // Read Y0 & Y1 Data
    dataY0 = readSPI(DATAY0_REG);
    dataY1 = readSPI(DATAY1_REG);
    // Combine Y0 and Y1
    dataY = (dataY1 << 8) | dataY0;
    // Read X0 & Z1 Data
    dataZ0 = readSPI(DATAZ0_REG);
    dataZ1 = readSPI(DATAZ1_REG);
    // Combine X0 and X1
    dataZ = (dataZ1 << 8) | dataZ0;

    int *arr = (int *)malloc(3 * sizeof(int));
    arr[0] = dataX;
    arr[1] = dataY;
    arr[2] = dataZ;
    return arr;
}