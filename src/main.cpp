////////////////////////////////////////////////////////////////////////////////
// BEELOG V4 
////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "config.h"
#include "flash.h"
#include "readid.h"
#include "rtcsleep.h"
#include "sensors.h"

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
  initFlash();
  initRtc();
  initSensors();

  // init data
  readIdFromEepRom(coordinatorAddressString);
  
  // indicate setup is done
  digitalWrite(LED_BUILTIN, LOW); 
}

// Loop
void loop() {
  SerialMon.println(":: Loop");
  digitalWrite(LED_BUILTIN, HIGH); // indicate loop start

  // Set new alarm
  setRtcAlarm(SLEEPTIMER);

  // create a data object
  LocalData_t localData;
  // read sensors
  getCoordinatorData(&localData);
  getWeatherData(&localData);
  showLocalData(&localData);

  delay(1000);
  digitalWrite(LED_BUILTIN, LOW); // indicate loop stop
  
  // delay or sleep
  SerialMon.println(":: Wait");
  delay(DELAY_TIMER);
}
