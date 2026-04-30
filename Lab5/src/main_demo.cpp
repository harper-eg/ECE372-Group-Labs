#include <avr/io.h>
#include "i2c.h"


int main() {

//initial I2C module
initI2C();



while(1){
sendDataI2C(0x72, 0x81);  // address 0x72 and data 0x81


}
return 0;
}