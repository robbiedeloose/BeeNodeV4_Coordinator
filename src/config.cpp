#include "config.h"

void displayCoordinatorData(char* coordinatorAddressString) {
  SerialMon.println(F("BeeNode v4.0.1a"));
  SerialMon.print(F("Id: "));
  SerialMon.println(coordinatorAddressString);
} 

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


// display sleep interval
// display operator
// display mqtt settings
// what other options need to include
