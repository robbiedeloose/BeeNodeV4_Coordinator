#include "config.h"

void displayCoordinatorData(char* coordinatorAddressString) {
  SerialMon.println(F("BeeNode v4.0.1a"));
  SerialMon.print(F("Id: "));
  SerialMon.println(coordinatorAddressString);
} 

void setPinModes() {
  // Battery monitor
  pinMode(A5,INPUT);
  // Led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // Gprssleep
  pinMode(GSM_RESET_PIN, OUTPUT);
  digitalWrite(GSM_RESET_PIN, LOW);
  // Sleep enable
  pinMode(SLEEP_ENABLED, INPUT_PULLUP);
  digitalWrite(SLEEP_ENABLED, HIGH);
  // Wake reciever
  pinMode(WAKE_RECIEVER, OUTPUT);
  digitalWrite(WAKE_RECIEVER, HIGH);
}


// display sleep interval
// display operator
// display mqtt settings
// what other options need to include
