#ifndef PINS_H   /* Include guard */
#define PINS_H

#include <arduino.h>

///////////////////////// OPTIONS //////////////////////////////////////////////
#define SLEEPTIMER 1      // how often do we want to send data (in minutes)
#define STARTDELAY 20      // delay start of programm, needed for reprogamming when using sleep
#define DEBUG             // comment to use sleep
#define DELAY_TIMER 10000  
///////////////////////// PIN DEFINES //////////////////////////////////////////
#define flashChipCSPin 4
#define buildInLed 13

#define GSM_RESET_PIN 2

#define SCALE_1_CLOCK 7  // wit-blauw 
#define SCALE_1_DATA 8   // blauw
#define SCALE_2_CLOCK 9  // wit-groen
#define SCALE_2_DATA 10   // groen
#define SCALE_3_CLOCK 11  // wit-bruin
#define SCALE_3_DATA 12   // bruin

#define SCALE_4_CLOCK 0  // geel
#define SCALE_4_DATA 1   // blauw
#define SCALE_5_CLOCK A1 // geel
#define SCALE_5_DATA A2  // blauw
#define SCALE_6_CLOCK A3 // geel
#define SCALE_6_DATA A4  // blauw

#endif