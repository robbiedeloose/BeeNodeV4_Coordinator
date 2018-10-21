#ifndef CONFIG_H   /* Include guard */
#define CONFIG_H

#include <arduino.h>

#define SerialMon SerialUSB
#define SerialAT Serial

#define CONTAINS_REFERENCE_SCALE
#define SCALE_SAMPLE_RATE 10
#define SLEEPTIMER 1      // how often do we want to send data (in minutes)
#define STARTDELAY 5      // delay start of programm, needed for reprogamming when using sleep
#define DEBUG             // comment to use sleep
#define DELAY_TIMER 10000  

void displayCoordinatorData(char* coordinatorAddressString);

#endif
