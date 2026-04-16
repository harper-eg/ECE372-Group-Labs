#ifndef I2C_H

#define I2C_H

void initI2C();
void sendDataI2C(unsigned char add,unsigned char internal, unsigned char send);
void read(unsigned char add, unsigned char internal);
#endif