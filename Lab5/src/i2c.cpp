#include "i2c.h"
#include <avr/io.h>
#include "timer.h"
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



void read(unsigned char add, unsigned char internal){

unsigned char var; 

TWCR = (1<< TWINT) | (1<< TWSTA) | (1<< TWEN); //resets flag

while (!(TWCR & (1<<TWINT))); //wait while flag is low
TWDR = (add << 1) |0; //load address and write bit(0)
TWCR = (1<<TWINT) | (1<<TWEN); //clear flag and enable TWI

while(!(TWCR & (1<<TWINT)));
TWDR = internal; 
TWCR = (1<<TWINT) | (1<<TWEN);

while(!(TWCR & (1<<TWINT)));
TWCR = (1<<TWEN) | (1<<TWINT) | (1<<TWSTA); //another start con

while (!(TWCR & (1<<TWINT))); //wait while flag is low
TWDR = (add << 1) |1; //load address and write bit(0)
TWCR = (1<<TWINT) | (1<<TWEN); //clear flag and enable TWI

while(!(TWCR & (1<<TWINT)));
var = (TWDR << 8);
TWCR = (1<<TWINT) | (1<<TWEN);

while(!(TWCR & (1<<TWINT)));
TWCR = (1<<TWEA) | (1<<TWINT) | (1<<TWEN);

while(!(TWCR & (1<<TWINT)));
var += TWDR; 
TWCR = (1<<TWINT) | (1<<TWEN); 

 while (!(TWCR & (1<<TWINT))); // wait for flag to set
 TWCR = (1<<TWINT)|(1<<TWEN)|(1<< TWSTO); //send stop condition
}


// 6. unsigned char Read_data()
// - Return TWDR
unsigned char read_data() {
  return TWDR; // Return the data from the two wire data register
}