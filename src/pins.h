#ifndef PINS_H   /* Include guard */
#define PINS_H

#include <arduino.h>


///////////////////////// PIN DEFINES //////////////////////////////////////////
#define flashChipCSPin  4
#define buildInLed      13

#define GSM_RESET_PIN 3

#define SCALE_1_CLOCK 7   // wit-blauw
#define SCALE_1_DATA  8   // blauw
#define SCALE_2_CLOCK 9   // wit-groen
#define SCALE_2_DATA  10  // groen
#define SCALE_3_CLOCK 11  // wit-bruin
#define SCALE_3_DATA  12  // bruin

#define SCALE_4_CLOCK 5   //0   // geel
#define SCALE_4_DATA  6   //1   // blauw
#define SCALE_5_CLOCK 14  // geel
#define SCALE_5_DATA  15  // blauw
#define SCALE_6_CLOCK 16  //A3  // geel
#define SCALE_6_DATA  17  //A4  // blauw

#define SCALE_REF_CLOCK 18  // geel
#define SCALE_REF_DATA  38  // blauw

#define SLEEP_ENABLED 2

struct LocalData_t {
  int16_t baseTemp;
  uint16_t baseHum;
  uint16_t baseLux;
  uint16_t baseBat;
  long int weights[6];
  long int refWeight;
};

void setPinModes();

#endif