// Initialization for I2C and a function sendatai2c that sends data using I2C
#include <avr/io.h>
#include "i2c.h"



void initI2C() 
{
  // wake up from power reduction:   if the I2C is in power reduction mode (PRTWI bit is 1) 
  //then it needs to be restored by writing a zero to PRTWI.
  
PRR0 &= ~(1 << PRTWI);  //On pg. 236 of ATmega2560 Datasheet   
  
  // generate a 100kHz clock rate
  // SCL freq = 16Mhz/(16 + 2(TWBR)*4)
  // Table 24-7 Prescaler value is 4 so TWPS0 = 1 and TWPS1 = 0.
  TWSR |= (1 << TWPS0);
  TWSR &= ~(1 << TWPS1);
  TWBR = 0x12; // bit rate generator = 100k  (TWBR = 18)

  TWCR |= (1 << TWINT | 1 << TWEN); // enable two wire interface
 

  
}

void sendDataI2C(unsigned char i2c_address, unsigned char data_to_send) {
 
 TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // reset flag and intiate a start condition

 while (!(TWCR & (1<<TWINT)));  // wait while flag is low
 TWDR = (i2c_address << 1) & (0xFE); //load i2c address and write bit (0)
 TWCR = (1<<TWINT)|(1<<TWEN);  // clear flag and enable TWI

 while (!(TWCR & (1<<TWINT))); // wait for flag to set
 TWDR = data_to_send; // load the data register with data
 TWCR = (1<<TWINT)|(1<<TWEN);  // clear flag and enable TWI

 while (!(TWCR & (1<<TWINT))); // wait for flag to set
 TWCR = (1<<TWINT)|(1<<TWEN)|(1<< TWSTO); //send stop condition
}
