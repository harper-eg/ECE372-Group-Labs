#ifndef I2C_H
#define I2C_H

void initI2C();
void sendDataI2C(unsigned char i2c_address, unsigned char data_to_send);
#endif
