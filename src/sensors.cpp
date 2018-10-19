
#define SerialMon SerialUSB
#define SerialAT Serial

#include <arduino.h>
#include "sensors.h"
#include "pins.h"
/******************************* Sensors **************************************/
//////////////////////////////// HUMIDITY //////////////////////////////////////
#include "SparkFunHTU21D.h"
HTU21D myHumidity;
//////////////////////////////// LUX ///////////////////////////////////////////
#include <BH1750.h>
BH1750 lightMeter;

////////////////////////////////// SCALES ////////////////////////////////////////
#include "HX711.h"

// DT, SCK  // parameter "gain" is ommited; the default value 128 is used by the library
HX711 scale1(SCALE_1_DATA, SCALE_1_CLOCK); 
HX711 scale2(SCALE_2_DATA, SCALE_2_CLOCK); 
HX711 scale3(SCALE_3_DATA, SCALE_3_CLOCK);
HX711 scale4(SCALE_4_DATA, SCALE_4_CLOCK); 
HX711 scale5(SCALE_5_DATA, SCALE_5_CLOCK); 
HX711 scale6(SCALE_6_DATA, SCALE_6_CLOCK); 


void initSensors() {
    myHumidity.begin();
    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
}

void getWeatherData(LocalData_t *local) {
    SerialMon.println(":: getWeatherData");
    SerialMon.println(" temp");
    local->baseTemp = myHumidity.readTemperature() * 100;
    SerialMon.println(" hum");
    local->baseHum = myHumidity.readHumidity() * 100;
    SerialMon.println(" lux");
    local->baseLux = lightMeter.readLightLevel();
}

void getScaleData(LocalData_t *local) {
    SerialMon.println(":: getScaleData");
    delay(1000);
    //SerialMon.println("scale1");
    local->weights[0] = scale1.get_value(10);
    //SerialMon.println("scale2");
    local->weights[1] = scale2.get_value(10);
    //SerialMon.println("scale3");
    local->weights[2] = scale3.get_value(10);
    //SerialMon.println("scale4");
    local->weights[3] = scale4.get_value(10);
    //SerialMon.println("scale5");
    local->weights[4] = scale5.get_value(10);
    //SerialMon.println("scale6");
    local->weights[5] = scale6.get_value(10);
    //SerialMon.println("done");
}