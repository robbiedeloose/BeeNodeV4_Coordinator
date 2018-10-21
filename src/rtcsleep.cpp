#include "rtcsleep.h"

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

void delayStartup() {
  /***** IMPORTANT DELAY FOR CODE UPLOAD BEFORE USB PORT DETACH DURING SLEEP *****/
  for(uint8_t i = 0; i < (STARTDELAY*2)+1; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    SerialMon.print(".");
    delay(500);
  }
  SerialMon.println();
}
