#include <Arduino.h>



// read address is 7-bit I2C address for the device is 0x1D
// POwer_CTL register D0 =0, D1=0,D2 = 0, D3 = 1, D4 = 0, D5 = 0

// 1. Two state machines are used for debouncing the switch and for
// implementing the display of the 8x8 LED smiley or frowny face.

// 2. If the accelerometer movement reaches a defined threshold value, a
// piezo alarm will trigger and produce a chirping sound. Your group will
// determine the threshold value to trigger the piezo through
// experimentation.

// 3. Once the piezo alarm is triggered it will remain on until it is
// silenced by pressing a button switch.

// 4. The x,y and z data from the accelerometer will be read out in the
// while loop in main()
 
// Read the datasheet and obtain necessary register numbers (hex)
 
// Initialize I2C process
 
// sei() <- make sure this is in there so that Serial.println works
 
// Start the I2C transmission to the SLA
 
// Write to power management -> write wakeup command (all zeros)
 
// Inside the while(1)
 
// Read from X high register -> Read from X low register
 
// Read from Y high register -> Read from Y low register
 
// Read from Z high register -> Read from Z low register
 
// Combine registers and serial print information
 
// Stop the I2C trans