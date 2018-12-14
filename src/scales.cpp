#include <arduino.h>
#include "scales.h"
#include "config.h"

HX711 scale1; 
HX711 scale2; 
HX711 scale3;
HX711 scale4; 
HX711 scale5; 
HX711 scale6; 

void initScales() {
  SerialMon.print("Init Scales ");
  SerialMon.print("1, ");
  scale1.begin(SCALE_1_DATA, SCALE_1_CLOCK);
  /*
  SerialMon.print("2, ");
  scale2.begin(SCALE_2_DATA, SCALE_2_CLOCK); 
  //SerialMon.print("3, ");
  scale3.begin(SCALE_3_DATA, SCALE_3_CLOCK);
  SerialMon.print("4, ");
  scale4.begin(SCALE_4_DATA, SCALE_4_CLOCK); 
  SerialMon.print("5, ");
  scale5.begin(SCALE_5_DATA, SCALE_5_CLOCK); 
  SerialMon.print("6 ");
  scale6.begin(SCALE_6_DATA, SCALE_6_CLOCK); 
  SerialMon.print("- done");
  */
}

void getScaleData(LocalData_t *local, uint8_t numberOfReads) {
  HX711 scales[] = {scale1, scale2, scale3, scale4, scale5, scale6};
  SerialMon.print(F(":: getScaleData - "));
  // fill with some data
  /*for(size_t num = 0; num < 6; num++) {
    SerialMon.print(num);
    SerialMon.print(F(", "));
    local->weights[num] = 7654321;//scales[num].get_value(numberOfReads);
  }*/
  // get one real scale
  local->weights[0] = scales[0].get_value(numberOfReads);
  SerialMon.println(F("done")); 
}