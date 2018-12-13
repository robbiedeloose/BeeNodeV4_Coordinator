////////////////////////////////////////////////////////////////////////////////
// BEELOG V4 
////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "config.h"
#include "flash.h"
#include "readid.h"
#include "rtcsleep.h"
#include "sensors.h"
#include "gprs.h"
#include "reciever.h"
#include "scales.h"

// Global Variables
char coordinatorAddressString[17] = "";

// Setup
void setup() {
  setPinModes();
  digitalWrite(LED_BUILTIN, HIGH); // indicate setup is started
    
  // Start Wire and Serial
  Wire.begin();
  SerialMon.begin(115200);
  SerialAT.begin(115200);
  //delay(5000); // Give serial time to start
  delayStartup();
  SerialMon.println(":: Setup");
  delay(1000);
  
  // read config data
  readIdFromEepRom(coordinatorAddressString);
  
  // external inits
  initFlash();
  initRtc();
  initSensors();
  //initScales();
  // init communications
  mqttInit(coordinatorAddressString);
  gprsResetModem(); // change by poweron
  mqttRegister(coordinatorAddressString);

  
  
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
  HiveData_t hiveDataBuffer;
  clearHiveBuffer(&hiveDataBuffer);

  // read sensors
  getCoordinatorData(&localData);
  getWeatherData(&localData);
  getDataFromReciever(&hiveDataBuffer);
  //getScaleData(&localData, SCALE_SAMPLE_RATE);
  // show data
  showLocalData(&localData);
  displayHiveBuffer(&hiveDataBuffer);
  // send data
  mqttSendData(&localData, &hiveDataBuffer);

  delay(1000);
  digitalWrite(LED_BUILTIN, LOW); // indicate loop stop
  
  // delay or sleep
  SerialMon.println(":: Wait");
  delay(DELAY_TIMER);
}
