#ifndef CONFIG_H   /* Include guard */
#define CONFIG_H

#include <arduino.h>

#define SerialMon SerialUSB
#define SerialAT Serial1

#define CONTAINS_REFERENCE_SCALE
#define SCALE_SAMPLE_RATE 10
#define SLEEPTIMER 5        // how often do we want to send data (in minutes)
#define STARTDELAY 10       // delay start of programm, needed for reprogamming when using sleep
#define DEBUG               // comment to use sleep
#define DELAY_TIMER 60000  
#define SCALE_CALIBRATION true

void displayCoordinatorData(char* coordinatorAddressString);

#endif
