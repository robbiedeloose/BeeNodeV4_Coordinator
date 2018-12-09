#include <arduino.h>
#include "sensors.h"
#include "config.h"

HTU21D myHumidity;

BH1750 lightMeter;

boolean errorBH1750 = false;

void initSensors() {  
  SerialMon.print("Init Sensors - HTU21, ");
  myHumidity.begin();
  SerialMon.print("BH1750, ");
  boolean e = lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  if (e) {
    SerialMon.println(" - done");
  } else {
    errorBH1750 = true;
    SerialMon.println(" - HB1750 init failed");
  }
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
  if (!errorBH1750){
    local->baseLux = lightMeter.readLightLevel();
  } else {
    local->baseLux = 0;
  }
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
  /*SerialMon.print(F("Scale1: "));
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
  SerialMon.println(local->weights[5]);*/
}
