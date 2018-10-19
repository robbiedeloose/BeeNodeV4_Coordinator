#include "config.h"

void displayCoordinatorData(char* coordinatorAddressString) {
    SerialMon.println(F("BeeNode v4.0.1a"));
    SerialMon.print("Id: ");
    SerialMon.println(coordinatorAddressString);
} 