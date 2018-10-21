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
HX711 scaleRef(SCALE_REF_DATA, SCALE_REF_CLOCK);

void initSensors() {
  myHumidity.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
}

void getCoordinatorData(LocalData_t *local) {
  SerialMon.print(":: getCoordinatorData - ");
  local->baseBat = analogRead(A5)*4.3;
  SerialMon.println("done");
}

void getWeatherData(LocalData_t *local) {
  SerialMon.print(":: getWeatherData - ");
  SerialMon.print("temp, ");
  local->baseTemp = myHumidity.readTemperature() * 100;
  SerialMon.print("hum, ");
  local->baseHum = myHumidity.readHumidity() * 100;
  SerialMon.print("lux, ");
  local->baseLux = lightMeter.readLightLevel();
  SerialMon.println("done");
}

void getScaleData(LocalData_t *local) {
  HX711 scales[] = {scale1, scale2, scale3, scale4, scale5, scale6};
  SerialMon.print(":: getScaleData - ");
  for(size_t num; num < scales.size(); num++) {
    SerialMon.print(num);
    SerialMon.print(", ");
    local->weights[num] = scales[num].get_value(SCALE_SAMPLE_RATE);
  }
  SerialMon.print("ref, ");
  local->refWeight = scaleRef.get_value(SCALE_SAMPLE_RATE);
  SerialMon.println("done");
}

void printLocalDataItem(const char[] label, const char[] value) {
  SerialMon.print(label);
  SerialMon.println(value);
}

void showLocalData(LocalData_t *local) {
  SerialMon.println(":: showLocalData ::");
  printLocalDataItem("Temp:\t", local->baseTemp);
  printLocalDataItem("Hum:\t", local->baseHum);
  printLocalDataItem("Lux:\t", local->baseLux);
  printLocalDataItem("Bat:\t", local->baseBat);
  printLocalDataItem("Scale1:\t", local->weights[0]);
  printLocalDataItem("Scale2:\t", local->weights[1]);
  printLocalDataItem("Scale3:\t", local->weights[2]);
  printLocalDataItem("Scale4:\t", local->weights[3]);
  printLocalDataItem("Scale5:\t", local->weights[4]);
  printLocalDataItem("Scale6:\t", local->weights[5]);
  printLocalDataItem("ScaleRef:\t", local->refWeight);
}
