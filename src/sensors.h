#ifndef SENSOR_H   /* Include guard */
#define SENSOR_H

#include <arduino.h>
#include <Wire.h>

 struct LocalData_t {
    int16_t baseTemp;
    uint16_t baseHum;
    uint16_t baseLux;
    uint16_t baseBat;
    long int weights[6];
  };

    void initSensors();
    void getCoordinatorData(LocalData_t *local);
    void getWeatherData(LocalData_t *local);
    void getScaleData(LocalData_t *local);
    void showLocalData(LocalData_t *local) ;

#endif