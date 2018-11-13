#include <arduino.h>
#include "sensors.h"
#include "pins.h"
#include "config.h"
/* Humidity and temp */
#include "SparkFunHTU21D.h"
HTU21D myHumidity;
/* Lux */
#include <BH1750.h>
BH1750 lightMeter;
/* Scales */
#include "HX711.h"
// DT, SCK  // parameter "gain" is ommited; the default value 128 is used by the library
HX711 scale1(SCALE_1_DATA, SCALE_1_CLOCK); 
HX711 scale2(SCALE_2_DATA, SCALE_2_CLOCK); 
HX711 scale3(SCALE_3_DATA, SCALE_3_CLOCK);
HX711 scale4(SCALE_4_DATA, SCALE_4_CLOCK); 
HX711 scale5(SCALE_5_DATA, SCALE_5_CLOCK); 
HX711 scale6(SCALE_6_DATA, SCALE_6_CLOCK); 
//HX711 scaleRef(SCALE_REF_DATA, SCALE_REF_CLOCK);

void initSensors() {
  myHumidity.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
}

void getCoordinatorData(LocalData_t *local) {
  SerialMon.print(F(":: getCoordinatorData - "));
  local->baseBat = analogRead(A5)*4.3;
  SerialMon.println(F("done"));
}

void getWeatherData(LocalData_t *local) {
  SerialMon.print(F(":: getWeatherData - "));
  SerialMon.print(F("temp, "));
  local->baseTemp = myHumidity.readTemperature() * 100;
  SerialMon.print(F("hum, "));
  local->baseHum = myHumidity.readHumidity() * 100;
  SerialMon.print(F("lux, "));
  local->baseLux = lightMeter.readLightLevel();
  SerialMon.println(F("done"));
}

void getScaleData(LocalData_t *local, uint8_t numberOfReads) {
  HX711 scales[] = {scale1, scale2, scale3, scale4, scale5, scale6};
  SerialMon.print(F(":: getScaleData - "));
  // fill with some data
  for(size_t num = 0; num < 6; num++) {
    SerialMon.print(num);
    SerialMon.print(F(", "));
    local->weights[num] = 7654321;//scales[num].get_value(numberOfReads);
  }
  // get one real scale
  local->weights[0] = scales[0].get_value(numberOfReads);
  SerialMon.println(F("done")); 
}

void showLocalData(LocalData_t *local) {
  SerialMon.println(F(":: showLocalData ::"));
  SerialMon.print(F("Temp:\t"));
  SerialMon.println(local->baseTemp);
  SerialMon.print(F("Hum:\t"));
  SerialMon.println(local->baseHum);
  SerialMon.print(F("Lux:\t"));
  SerialMon.println(local->baseLux);
  SerialMon.print(F("Bat:\t"));
  SerialMon.println(local->baseBat);
  SerialMon.print(F("Scale1: "));
  SerialMon.println(local->weights[0]);
  SerialMon.print(F("Scale2: "));
  SerialMon.println(local->weights[1]);
  SerialMon.print(F("Scale3: "));
  SerialMon.println(local->weights[2]);
  SerialMon.print(F("Scale4: "));
  SerialMon.println(local->weights[3]);
  SerialMon.print(F("Scale5: "));
  SerialMon.println(local->weights[4]);
  SerialMon.print(F("Scale6: "));
  SerialMon.println(local->weights[5]);
}
