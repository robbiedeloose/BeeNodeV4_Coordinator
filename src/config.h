#ifndef CONFIG_H   /* Include guard */
#define CONFIG_H

#define SerialMon SerialUSB
#define SerialAT Serial

#include <arduino.h>

void displayCoordinatorData(char* coordinatorAddressString);

#endif