// I2C Functions:

// 1. InitI2C()
// - Wake up I2C module on mega 2560
// - Set prescaler TWPS to 1
// - Set two wire interface bit rate register TWBR
// - Enable two wire interface

//set freq 100 kHZ

// 2. StartI2C_Trans(unsigned char SLA)
// - Clear TWINT, initiate start condition, initiate enable
// - Wait for completion
// - Set two wire data register to the SLA + write bit
// - Trigger action: Clear TWINT and initiate enable
// - Wait for completion

// 3. StopI2C_Trans()
// - Trigger action + stop condition

// 4. Write(unsigned char data)
// - Set two wire data register equal to incoming data
// - Trigger action
// - Wait for completion

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

// 6. unsigned char Read_data()
// - Return TWDR