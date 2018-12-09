#ifndef SENSOR_H   /* Include guard */
#define SENSOR_H

#include <arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include "SparkFunHTU21D.h"
#include "config.h"

void initSensors();
void getCoordinatorData(LocalData_t *local);
void getWeatherData(LocalData_t *local);
void getScaleData(LocalData_t *local, uint8_t numberOfReads);
void showLocalData(LocalData_t *local);

#endif
