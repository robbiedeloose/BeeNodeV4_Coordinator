#ifndef SCALES_H   /* Include guard */
#define SCALES_H

#include <arduino.h>
#include "config.h"
/* Scales */
#include "HX711.h"
// DT, SCK  // parameter "gain" is ommited; the default value 128 is used by the library

void initScales();
void getScaleData(LocalData_t *local, uint8_t numberOfReads);

#endif