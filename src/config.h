#ifndef CONFIG_H   /* Include guard */
#define CONFIG_H

#include <arduino.h>

#define SerialMon SerialUSB
#define SerialAT Serial1

#define CONTAINS_REFERENCE_SCALE
#define SCALE_SAMPLE_RATE 15
#define SLEEPTIMER 1        // how often do we want to send data (in minutes)
#define STARTDELAY 10       // delay start of programm, needed for reprogamming when using sleep
#define DEBUG               // comment to use sleep
#define DELAY_TIMER 125000   // in miliseconds
#define HIVE_BUFFER 6

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

#define WAKE_RECIEVER 18

#define SLEEP_ENABLED 2

struct LocalData_t {
  int16_t baseTemp;
  uint16_t baseHum;
  uint16_t baseLux;
  uint16_t baseBat;
  long int weights[6];
};

struct HiveData_t {
  char id[6][9];
  int temp[6];
  int hum[6];
  int bat[6];
  int weight[6];
};

void setPinModes();
void displayCoordinatorData(char* coordinatorAddressString);

#endif
