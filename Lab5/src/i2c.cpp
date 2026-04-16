#include "i2c.h"
#include <avr/io.h>

#include "i2c.h"
#include <avr/io.h>
#include "timer.h"
// I2C Functions:


void initI2C(){
   PRR0 &= ~(1 << PRTWI) //wakes up power 

   TWCR |= (1<<TWEN) | (1<<TWIE);
   TWSR |= (1<<TWPS0); //sets prescaler to 4^1

   TWBR = 18; //for100kHZ 

   TWCR |= (1<<TWIE) | (1<<TWEN); // 2 wire interface 

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
// 1. InitI2C()
// - Wake up I2C module on mega 2560
// - Set prescaler TWPS to 1
// - Set two wire interface bit rate register TWBR
// - Enable two wire interface

void initI2C() {
  // Set prescaler TWPS to 1
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1));
  
  // Set two wire interface bit rate register TWBR
  TWBR = 72; // Set bit rate to 100 kHz (assuming F_CPU = 16 MHz)
  
  // Enable two wire interface
  TWCR = (1 << TWEN);
}

//set freq 100 kHZ

// 2. StartI2C_Trans(unsigned char SLA)
// - Clear TWINT, initiate start condition, initiate enable
// - Wait for completion
// - Set two wire data register to the SLA + write bit
// - Trigger action: Clear TWINT and initiate enable
// - Wait for completion

void startI2C_Trans(unsigned char SLA) {
  // Clear TWINT, initiate start condition, initiate enable
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  
  // Wait for completion
  while (!(TWCR & (1 << TWINT)));
  
  // Set two wire data register to the SLA + write bit
  TWDR = (SLA << 1); // Shift SLA left and add write bit (0)
  
  // Trigger action: Clear TWINT and initiate enable
  TWCR = (1 << TWINT) | (1 << TWEN);
  
  // Wait for completion
  while (!(TWCR & (1 << TWINT)));
}

// 3. StopI2C_Trans()
// - Trigger action + stop condition

void stopI2C_Trans() {
  // Trigger action + stop condition
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// 4. Write(unsigned char data)
// - Set two wire data register equal to incoming data
// - Trigger action
// - Wait for completion

void write(unsigned char data) {
  // Set two wire data register equal to incoming data
  TWDR = data;
  
  // Trigger action
  TWCR = (1 << TWINT) | (1 << TWEN);
  
  // Wait for completion
  while (!(TWCR & (1 << TWINT)));
}

// 5. Read_from(unsigned char SLA, unsigned Char MEMADDRESS)
// - Start a transmission to the SLA
// - Write to the MEMADDRESS
// - Clear TWINT, initiate start condition, initiate enable
// - Wait for completion
// - Set two wire data register to the SLA + read bit
// - Trigger action + master acknowledge bit
// - Wait for completion
// - Trigger action
// - Wait for completion
// - Trigger action + stop condition

void read_from(unsigned char SLA, unsigned char MEMADDRESS) {
  // Start a transmission to the SLA
  startI2C_Trans(SLA);

  write(MEMADDRESS); // Write to the MEMADDRESS
  // Clear TWINT, initiate start condition, initiate enable
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  // Wait for completion
  delayMs(10); // Add a small delay to ensure the slave has time to respond
  // Set two wire data register to the SLA + read bit
  TWDR = (SLA << 1) | 1; // Shift SLA left and add read bit (1)
  // Trigger action + master acknowledge bit
  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
  // Wait for completion
  delayMs(10); // Add a small delay to ensure the slave has time to respond
  // Trigger action + stop condition
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}  

// 6. unsigned char Read_data()
// - Return TWDR
unsigned char read_data() {
  return TWDR; // Return the data from the two wire data register
}