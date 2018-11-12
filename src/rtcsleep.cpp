#include "rtcsleep.h"

RTCZero rtc;

void initRtc() {
  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);
}

void setRtcAlarm(uint8_t alarmMinutes) {
  SerialMon.print(":: setRtcAlarm - next: ");
  rtc.setAlarmSeconds(00);
  rtc.setAlarmMinutes((rtc.getMinutes()+alarmMinutes)%60);
  SerialMon.print(rtc.getMinutes()+alarmMinutes)%60);
  rtc.enableAlarm(rtc.MATCH_MMSS);
  rtc.attachInterrupt(alarmMatch);
  SerialMon.println(" done");
}

void sleepCoordinator() {
  SerialMon.println(":: sleepCoordinator");
  delay(500); // give SerialMon time to complete before node goes to sleep
  USBDevice.detach();
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

void checkSleepEnabled(){

}