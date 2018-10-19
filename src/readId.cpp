#include "readId.h"

void readIdFromEepRom(char* coordinatorAddressString) {
     uint8_t coordinatorAddress[8];
    //0x50 is the I2c address, 0xF8 is the memory address where the read-only MAC value is
    Wire.beginTransmission(0x50);
    Wire.write(0xF8); // LSB 
    Wire.endTransmission();
    Wire.requestFrom(0x50, 8); //request 8 bytes from the device
    uint8_t i = 0;
    while (Wire.available()){ 
        coordinatorAddress[i] = Wire.read();
        i++;
    }
    sprintf(coordinatorAddressString, "%02X%02X%02X%02X%02X%02X%02X%02X", coordinatorAddress[0], coordinatorAddress[1], coordinatorAddress[2], coordinatorAddress[3], coordinatorAddress[4], coordinatorAddress[5], coordinatorAddress[6], coordinatorAddress[7]);
}