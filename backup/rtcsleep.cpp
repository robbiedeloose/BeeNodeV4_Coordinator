#include "rtcsleep.h"

RTCZero rtc;

void initRtc() {
  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);
}

void setRtcAlarm(uint8_t alarmMinutes) {
  SerialMon.print(F(":: setRtcAlarm - next: "));
  rtc.setAlarmSeconds(00);
  uint8_t min = (rtc.getMinutes()+alarmMinutes)%60;
  SerialMon.print(min);
  rtc.setAlarmMinutes(min);
  
  rtc.enableAlarm(rtc.MATCH_MMSS);
  rtc.attachInterrupt(alarmMatch);
  SerialMon.println(F(" done"));
}

void sleepCoordinator() {
  SerialMon.println(F(":: sleepCoordinator"));
  delay(500); // give SerialMon time to complete before node goes to sleep
  USBDevice.detach();
  rtc.standbyMode();
}

void alarmMatch() {
}

void delayStartup() {
  /***** IMPORTANT DELAY FOR CODE UPLOAD BEFORE USB PORT DETACH DURING SLEEP *****/
  for(uint8_t i = 0; i < (STARTDELAY*2)+1; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    SerialMon.print(F("."));
    delay(500);
  }
  SerialMon.println();
}

void checkSleepEnabled(){

}