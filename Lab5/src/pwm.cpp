// Authors:
// Net IDs:
// Date:
// Assignment: Lab 5
//
// PWM driver for the piezo buzzer chirping alarm.
//   Timer4 generates a variable-frequency square wave on OC4B (PH4) using
//   Fast PWM mode 15 (OCR4A = TOP, OCR4B = compare). Changing OCR4A changes
//   the tone frequency; OCR4B is held at OCR4A/2 for 50% duty cycle.
//   Timer3 fires a CTC ISR every ~100 ms that swaps the tone between two
//   frequencies, producing the audible chirp.
//----------------------------------------------------------------------//

#include "pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Fast PWM TOP values for OC4B at prescaler /8: f = 2 MHz / (1 + OCR4A)
#define CHIRP_TOP_LOW    999    // ~2.0 kHz
#define CHIRP_TOP_HIGH   499    // ~4.0 kHz

// Timer3 OCR for ~100 ms tick at prescaler /1024 (16 MHz / 1024 = 15625 Hz)
#define CHIRP_TICK_OCR   1561

static volatile bool chirpHigh = false;

static inline void setChirpTone(uint16_t top) {
    OCR4A = top;
    OCR4B = top >> 1;                       // 50% duty cycle
}

void initPWM() {
    // OC4B drives the piezo
    DDRH |= (1 << DDH4);

    // Timer4: Fast PWM, mode 15 (WGM4[3:0] = 1111), OCR4A as TOP.
    // COM4B[1:0] = 10 → non-inverting PWM on OC4B (clear on match, set at TOP).
    // Clock stays stopped until startChirp() turns the tone on.
    TCCR4A = (1 << COM4B1) | (1 << WGM41) | (1 << WGM40);
    TCCR4B = (1 << WGM43)  | (1 << WGM42);
    setChirpTone(CHIRP_TOP_LOW);

    // Timer3: CTC mode (WGM3[3:0] = 0100). Used purely as a 100 ms tick to
    // flip the chirp tone — no compare-output pin needed.
    TCCR3A = 0;
    TCCR3B = (1 << WGM32);
    OCR3A  = CHIRP_TICK_OCR;
}

void startChirp() {
    chirpHigh = false;
    setChirpTone(CHIRP_TOP_LOW);
    TCNT4 = 0;
    TCNT3 = 0;

    // Re-attach OC4B in case stopChirp() detached it.
    TCCR4A |= (1 << COM4B1);
    TCCR4A &= ~(1 << COM4B0);

    // Start Timer4 with prescaler /8 → audible tone.
    TCCR4B |= (1 << CS41);

    // Start Timer3 with prescaler /1024 and enable the chirp-toggle ISR.
    TIMSK3 |= (1 << OCIE3A);
    TCCR3B |= (1 << CS32) | (1 << CS30);
}

void stopChirp() {
    // Detach OC4B so the pin can be driven low manually, then stop Timer4.
    TCCR4A &= ~((1 << COM4B1) | (1 << COM4B0));
    TCCR4B &= ~((1 << CS42) | (1 << CS41) | (1 << CS40));
    PORTH  &= ~(1 << PORTH4);

    // Stop Timer3 chirp-toggle ISR.
    TCCR3B &= ~((1 << CS32) | (1 << CS31) | (1 << CS30));
    TIMSK3 &= ~(1 << OCIE3A);
}

ISR(TIMER3_COMPA_vect) {
    chirpHigh = !chirpHigh;
    setChirpTone(chirpHigh ? CHIRP_TOP_HIGH : CHIRP_TOP_LOW);
}
