#include "config.h"

void displayCoordinatorData(char* coordinatorAddressString) {
  SerialMon.println(F("BeeNode v4.0.1a"));
  SerialMon.print(F("Id: "));
  SerialMon.println(coordinatorAddressString);
} 

// display sleep interval
// display operator
// display mqtt settings
// what other options need to include
