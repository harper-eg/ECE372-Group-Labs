// Author:
// Net ID:
// Date:
// Assignment:     Lab 4
//
// Description: This file implements functions that utilize the timers
//----------------------------------------------------------------------//

#include "timer.h"

/* Initialize timer 0 for debouncing. CTC mode. */
void initTimer0(){
    // CTC SETTING IN THE TIMER REGISTER
    TCCR0A&=~(1 << WGM00);
    TCCR0A|=(1 << WGM01);
    TCCR0B&=~(1 << WGM02);

}

/* This delays the program an amount specified by unsigned int delay.
* Use timer 0. Prescalar of 64, precise to 1 millisecond.
*/
void delayMs(unsigned int delay){

    unsigned int count = 0;
    //delay will be measured in milliseconds, and with a prescalar of 64 so OCR0A will count to 1 ms every 250 counts

    OCR0A = 250; // Appropriate count value

    //Normal clock frequency of 16MHz
    //These statements starts the timer with the prescalar set to 64
    TCCR0B &= ~(1 << CS02);
    TCCR0B |= ((1 << CS01) | (1 << CS00));

    while(count < delay){ //if we still need to delay by more miliseconds

        TIFR0 |= (1 << OCF0A); //this sets the CTC flag down, so that we can start a new clock delay (flag down is logic 1)
        TCNT0 = 0; //clears the timer

        //while flag is down do not do anything
        while(!(TIFR0 & (1 << OCF0A)));

        count++; //increment counter
    }

    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); //turns timer off
}

/* Initialize timer 1 for the 10-second seven segment countdown.
*  Should use CTC mode with an interrupt to count 1-second intervals.
*/
void initTimer1(){
    // CTC mode (from Lab 3)
    TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
    TCCR1B |= (1 << WGM12);
    TCCR1B &= ~(1 << WGM13);

    // TODO: Set OCR1A for a 1-second interval using an appropriate prescaler
    // TODO: Enable Timer1 compare match A interrupt (OCIE1A in TIMSK1)
    // TODO: Set prescaler bits (CS12:CS10) for desired prescaler
}

// TODO: Implement any helper functions to start/stop Timer1 for the countdown

void startTimer1() {
    // Set OCR1A for 1-second interval with prescaler of 1024
    OCR1A = 15624; // (16MHz / 1024) - 1 = 15624 counts for 1 second

    // Enable Timer1 compare match A interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Start Timer1 with prescaler of 1024
    TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
}
