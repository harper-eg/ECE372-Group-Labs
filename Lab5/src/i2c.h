#ifndef I2C_H

#define I2C_H

// MPU-6050 register map
#define MPU6050_ADDR  0x68   // 7-bit slave address
#define PWR_MGMT_1    0x6B   // Power management register
#define ACCEL_XOUT_H  0x3B   // X-axis high byte
#define ACCEL_XOUT_L  0x3C   // X-axis low byte
#define ACCEL_YOUT_H  0x3D   // Y-axis high byte
#define ACCEL_YOUT_L  0x3E   // Y-axis low byte
#define ACCEL_ZOUT_H  0x3F   // Z-axis high byte
#define ACCEL_ZOUT_L  0x40   // Z-axis low byte

void initI2C();
void sendDataI2C(unsigned char add,unsigned char internal, unsigned char send);
unsigned char read(unsigned char add, unsigned char internal);
bool read_acceleration_threshold();



#endif