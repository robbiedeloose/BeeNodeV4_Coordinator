#ifndef SENSOR_H   /* Include guard */
#define SENSOR_H

#include <arduino.h>
#include <Wire.h>
#include "pins.h"

    void initSensors();
    void getCoordinatorData(LocalData_t *local);
    void getWeatherData(LocalData_t *local);
    void getScaleData(LocalData_t *local);
    void showLocalData(LocalData_t *local);

#endif