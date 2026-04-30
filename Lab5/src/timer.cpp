// Authors:
// Net IDs:
// Date:
// Assignment: Lab 5
//
// Precise millisecond timer used for switch debouncing.
// Timer 1 in CTC mode, prescaler 64 → 250 counts per ms at 16 MHz.
//----------------------------------------------------------------------//

#include "timer.h"

void initTimer1() {
    // CTC mode (WGM12 set, others cleared); clock stays stopped until delayMs.
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);
    OCR1A  = 250;                                   // 1 ms at /64
}

void delayMs(unsigned int delay) {
    OCR1A = 250;
    TCNT1 = 0;
    TIFR1 |= (1 << OCF1A);                          // clear stale flag

    // Start Timer 1 with prescaler /64.
    TCCR1B |= (1 << CS11) | (1 << CS10);
    TCCR1B &= ~(1 << CS12);

    for (unsigned int i = 0; i < delay; i++) {
        while (!(TIFR1 & (1 << OCF1A)));
        TIFR1 |= (1 << OCF1A);
    }

    // Stop Timer 1 so it stays free for any future use.
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}
