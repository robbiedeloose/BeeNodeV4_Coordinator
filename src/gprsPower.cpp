#include <arduino.h>
#include "gprsPower.h"

#define GSM_RESET_PIN 2

uint8_t gprsPowerOn(uint8_t powerstate){
    Serial.println(":: gprsPowerOn");
    if (powerstate == 0){
      // pull powerbutton low for 1,5 sec
      digitalWrite(GSM_RESET_PIN, LOW);
      delay(1500); // should replace this with a 1,5s sleep
      digitalWrite(GSM_RESET_PIN, HIGH);
    }
    return 1;
}

uint8_t gprsPowerOff(uint8_t powerstate){
    Serial.println(":: gprsPowerOff");
    if (powerstate == 1){
      // pull powerbutton low for 1,5 sec
      digitalWrite(GSM_RESET_PIN, LOW);
      delay(1500); // should replace this with a 1,5s sleep
      digitalWrite(GSM_RESET_PIN, HIGH);
    }
    return 0;
}
