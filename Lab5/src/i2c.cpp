#include "i2c.h"
#include <avr/io.h>
#include "timer.h"
#include <math.h>
#include <stdlib.h>

// MPU-6050 register addresses
#define MPU6050_ADDR  0x68
#define ACCEL_XOUT_H  0x3B
#define ACCEL_XOUT_L  0x3C
#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E
#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40

unsigned char accelX0 = 0; // Read X0
unsigned char accelX1 = 0; // Read X1
unsigned char accelY0 = 0; // Read Y0
unsigned char accelY1 = 0; // Read Y1
unsigned char accelZ0 = 0; // Read Z0
unsigned char accelZ1 = 0; // Read Z1

// I2C Functions:



// 1. InitI2C()
// - Wake up I2C module on mega 2560
// - Set prescaler TWPS to 1
// - Set two wire interface bit rate register TWBR
// - Enable two wire interface

void initI2C(){
   PRR0 &= ~(1 << PRTWI); //wakes up power 

   TWCR |= (1<<TWEN) | (1<<TWIE);
   TWSR |= (1<<TWPS0); //sets prescaler to 4^1

   TWBR = 18; //for100kHZ 

   TWCR |= (1<<TWINT) | (1<<TWEN); // 2 wire interface 
}

void sendDataI2C(unsigned char add, unsigned char internal, unsigned char send){
TWCR = (1<< TWINT) | (1<< TWSTA) | (1<< TWEN); //resets flag

while (!(TWCR & (1<<TWINT))); //wait while flag is low
TWDR = (add << 1) |0; //load address and write bit(0)
TWCR = (1<<TWINT) | (1<<TWEN); //clear flag and enable TWI

while(!(TWCR & (1<<TWINT)));

TWDR = internal; 
TWCR = (1<<TWINT) | (1<<TWEN);

while(!(TWCR & (1<<TWINT)));
TWDR = send; //load the data registerr 
TWCR = (1<<TWINT) | (1<<TWEN); // clear flag



 while (!(TWCR & (1<<TWINT))); // wait for flag to set
 TWCR = (1<<TWINT)|(1<<TWEN)|(1<< TWSTO); //send stop condition

}



unsigned char read(unsigned char add, unsigned char internal){
  
unsigned char data; 

TWCR = (1<< TWINT) | (1<< TWSTA) | (1<< TWEN); // start

while (!(TWCR & (1<<TWINT))); //wait while flag is low
TWDR = (add << 1) | 0; //load address and write bit(0)
TWCR = (1<<TWINT) | (1<<TWEN); //clear flag and enable TWI

while(!(TWCR & (1<<TWINT)));
TWDR = internal; // register address
TWCR = (1<<TWINT) | (1<<TWEN);

while(!(TWCR & (1<<TWINT)));
TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTA); // restart

while (!(TWCR & (1<<TWINT))); //wait while flag is low
TWDR = (add << 1) | 1; //load address and read bit(1)
TWCR = (1<<TWINT) | (1<<TWEN); //clear flag and enable TWI, no TWEA for NACK

while(!(TWCR & (1<<TWINT)));
data = TWDR; // read the data
TWCR = (1<<TWINT)|(1<<TWEN)|(1<< TWSTO); //send stop condition
return data;
}


bool read_acceleration_threshold() {
    // Read the acceleration data from the sensor
    accelX1 = read(MPU6050_ADDR, ACCEL_XOUT_H); // Read X high byte
    accelX0 = read(MPU6050_ADDR, ACCEL_XOUT_L); // Read X low byte
    accelY1 = read(MPU6050_ADDR, ACCEL_YOUT_H); // Read Y high byte
    accelY0 = read(MPU6050_ADDR, ACCEL_YOUT_L); // Read Y low byte
    accelZ1 = read(MPU6050_ADDR, ACCEL_ZOUT_H); // Read Z high byte
    accelZ0 = read(MPU6050_ADDR, ACCEL_ZOUT_L); // Read Z low byte

    // Combine the high and low bytes for each axis
    int accelX = ((int16_t)(int8_t)accelX1 << 8) | accelX0;
    int accelY = ((int16_t)(int8_t)accelY1 << 8) | accelY0;
    int accelZ = ((int16_t)(int8_t)accelZ1 << 8) | accelZ0;

    // Check if any of the acceleration values exceed the threshold
    if (abs(accelY) > 11000 || abs(accelZ) < 11000) {
        return true; // Threshold exceeded
    } else {
        return false; // Threshold not exceeded
    }
}