#include "rtcsleep.h"

RTCZero rtc;

void setRtcAlarm(uint8_t alarmMinutes) {
  SerialMon.print(":: setRtcAlarm - ");
  SerialMon.print("set minutes: ");
  rtc.setAlarmSeconds(15);
  rtc.setAlarmMinutes((rtc.getMinutes()+alarmMinutes)%60);
  SerialMon.print((rtc.getMinutes()+alarmMinutes)%60);
  SerialMon.print(", enable alarm, ");
  rtc.enableAlarm(rtc.MATCH_SS);
  SerialMon.print("attach interrupt, ");
  rtc.attachInterrupt(alarmMatch);
  SerialMon.println("done");
}

void sleepCoordinator() {
  SerialMon.println(":: sleepCoordinator");
  delay(500); // give SerialMon time to complete before node goes to sleep
  rtc.standbyMode();
}

void alarmMatch() {
    Serial.println(":: wake");
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
