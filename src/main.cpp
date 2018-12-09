////////////////////////////////////////////////////////////////////////////////
// BEELOG V4 
////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "config.h"
#include "flash.h"
#include "readid.h"

// Global Variables
char coordinatorAddressString[17] = "";

// Setup
void setup() {
  setPinModes();
  digitalWrite(LED_BUILTIN, HIGH); // indicate setup is started
    
  // Start Wire and Serial
  Wire.begin();
  SerialMon.begin(115200);
  delay(5000); // Give serial time to start
  SerialMon.println(":: Setup");
  delay(1000);
  
  // external inits
  SerialMon.print("Init Flash ");
  initFlash();
  SerialMon.println("- done");

  // init data
  SerialMon.print("Get Co Id ");
  readIdFromEepRom(coordinatorAddressString);
  SerialMon.println("- done");
  SerialMon.print("Id: ");
  SerialMon.println(coordinatorAddressString);

  digitalWrite(LED_BUILTIN, LOW); // indicate setup is done
}

// Loop
void loop() {
  SerialMon.println(":: Loop");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  SerialMon.println(":: Wait");
  digitalWrite(LED_BUILTIN, LOW);
  delay(DELAY_TIMER);
}
