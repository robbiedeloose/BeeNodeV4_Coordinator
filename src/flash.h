#ifndef FLASH_H   /* Include guard */
#define FLASH_H

#include <arduino.h>
#include <SPI.h>
#include <SerialFlash.h>
#include "pins.h"

const int flashChipSelect = flashChipCSPin;

void initFlash();

#endif