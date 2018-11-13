#ifndef RTCFUNCTIONS_H   /* Include guard */
#define RTCFUNCTIONS_H

#include <arduino.h>
#include <RTCZero.h>
  
#include "pins.h"
#include "config.h"

/* Change these values to set the current initial time */
const uint8_t seconds = 0;
const uint8_t minutes = 0;
const uint8_t hours = 10;
/* Change these values to set the current initial date */
const uint8_t day = 20;
const uint8_t month = 2;
const uint8_t year = 17;

void initRtc();
void setRtcAlarm(uint8_t alarmMinutes);
void sleepCoordinator();
void alarmMatch();
void delayStartup();
void checkSleepEnabled();

#endif
