#ifndef SWITCH_H
#define SWITCH_H

// Debounce state machine states. Owned by switch.cpp; the alarm-display
// state machine in main.cpp reads/clears this state.
typedef enum buttonState_enum {
    wait_press,
    debounce_press,
    wait_release,
    debounce_release
} buttonState;

extern volatile buttonState switchState;

// Configures the silence-button input + external interrupt. The default
// pin is PE4 (Arduino D2 / INT4) because PD0 is needed by I²C SCL on
// this Mega2560 board. Update SWITCH_* macros in switch.cpp if your
// hardware uses a different pin.
void initSwitch();

#endif
