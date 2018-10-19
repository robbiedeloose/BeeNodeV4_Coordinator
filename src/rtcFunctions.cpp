#include "rtcFunctions.h"
#define SerialMon SerialUSB

RTCZero rtc;

void setRtcAlarm(uint8_t alarmMinutes) {
  SerialMon.println(":: setRtcAlarm");
  rtc.setAlarmSeconds(0);
  rtc.setAlarmMinutes((rtc.getMinutes()+alarmMinutes)%60);
  rtc.enableAlarm(rtc.MATCH_MMSS);
  rtc.attachInterrupt(alarmMatch);
}

void sleepCoordinator() {
  SerialMon.println(F("sleep"));
  digitalWrite(LED_BUILTIN, LOW);
  delay(500); // give SerialMon time to complete before node goes to sleep
}

void alarmMatch() {
    
}