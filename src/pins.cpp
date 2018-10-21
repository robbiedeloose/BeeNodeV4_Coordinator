#include "pins.h"

void setPinModes() {
  pinMode(A5,INPUT);
  //led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //gprssleep
  pinMode(GSM_RESET_PIN, OUTPUT);
  digitalWrite(GSM_RESET_PIN, HIGH);
  //sleep enable
  pinMode(SLEEP_ENABLED, INPUT_PULLUP);
  digitalWrite(SLEEP_ENABLED, HIGH);
}
