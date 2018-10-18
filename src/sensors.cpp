
#define SerialMon SerialUSB
#define SerialAT Serial

#include <arduino.h>
#include "sensors.h"
/******************************* Sensors **************************************/
//////////////////////////////// HUMIDITY //////////////////////////////////////
#include "SparkFunHTU21D.h"
HTU21D myHumidity;
//////////////////////////////// LUX ///////////////////////////////////////////
#include <BH1750.h>
BH1750 lightMeter;

////////////////////////////////// SCALES ////////////////////////////////////////
#include "HX711.h"
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
    local->baseTemp = myHumidity.readTemperature() * 100;
    local->baseHum = myHumidity.readHumidity() * 100;
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