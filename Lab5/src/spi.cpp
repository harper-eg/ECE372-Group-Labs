// Authors:
// Net IDs:
// Date:
// Assignment: Lab 5
//
// SPI driver for the MAX7219-based 8x8 LED matrix.
//   CS   = PB4 (LOAD/!CS, driven as a regular GPIO)
//   SCK  = PB1   MOSI = PB2   MISO = PB3 (unused)
//   PB0  = AVR hardware SS — kept as an output (high) so the SPI peripheral
//          stays in master mode; not connected to the matrix.
//
// MAX7219 expects 16-bit packets (4 don't-care + 4 register-address +
// 8 data bits) sampled on the rising edge of SCK with idle-low clock —
// SPI mode 0 (CPOL=0, CPHA=0), MSB first.
//----------------------------------------------------------------------//

#include "spi.h"
#include <avr/io.h>

// MAX7219 control registers
#define MAX7219_NOOP          0x00
#define MAX7219_DECODE_MODE   0x09
#define MAX7219_INTENSITY     0x0A
#define MAX7219_SCAN_LIMIT    0x0B
#define MAX7219_SHUTDOWN      0x0C
#define MAX7219_DISPLAY_TEST  0x0F

// 8-row bitmaps for the two faces. Bit 7 of each byte is the leftmost column.
static const unsigned char SMILEY[8] = {
    0x3C,   // ..####..
    0x42,   // .#....#.
    0xA5,   // #.#..#.#  eyes
    0x81,   // #......#
    0xA5,   // #.#..#.#
    0x99,   // #..##..#  smile
    0x42,   // .#....#.
    0x3C    // ..####..
};

static const unsigned char FROWNY[8] = {
    0x3C,
    0x42,
    0xA5,
    0x81,
    0x99,   // frown is the smile row flipped vertically
    0xA5,
    0x42,
    0x3C
};

// Send one 16-bit MAX7219 packet (register + data) framed by CS (PB4).
// CS must stay low across both bytes; the rising edge latches the packet.
static void writeSPI(unsigned char regAddress, unsigned char data) {
    PORTB &= ~(1 << PORTB4);                // CS low: begin frame
    SPDR = regAddress;
    while (!(SPSR & (1 << SPIF)));
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    PORTB |= (1 << PORTB4);                 // CS high latches the packet
}

void initSPI() {
    // PB0 hardware SS, PB1 SCK, PB2 MOSI, PB4 CS as outputs; PB3 MISO input.
    DDRB |=  (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);
    DDRB &= ~(1 << DDB3);
    PORTB |= (1 << PORTB0) | (1 << PORTB4); // SS and CS idle high

    // SPI master, mode 0 (CPOL=0, CPHA=0), MSB first, fosc/16.
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    SPSR = 0;

    // MAX7219 power-up configuration.
    writeSPI(MAX7219_SHUTDOWN,     0x00);   // shutdown while configuring
    writeSPI(MAX7219_DISPLAY_TEST, 0x00);   // display test off
    writeSPI(MAX7219_DECODE_MODE,  0x00);   // raw bitmap mode (no BCD decode)
    writeSPI(MAX7219_SCAN_LIMIT,   0x07);   // scan all 8 digits/rows
    writeSPI(MAX7219_INTENSITY,    0x08);   // mid brightness
    writeSPI(MAX7219_SHUTDOWN,     0x01);   // normal operation

    for (unsigned char row = 0; row < 8; row++) {
        writeSPI(row + 1, 0x00);            // blank the matrix
    }
}

void write_LED(bool faceState) {
    const unsigned char *frame = faceState ? SMILEY : FROWNY;
    for (unsigned char row = 0; row < 8; row++) {
        writeSPI(row + 1, frame[row]);      // digit registers are 0x01..0x08
    }
}
