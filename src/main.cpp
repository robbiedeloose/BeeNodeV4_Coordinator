////////////////////////////////////////////////////////////////////////////////
// BEELOG V4 
////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "readId.h"
#include "sensors.h"
#include "rtcsleep.h"
#include "flash.h"
#include "gprs.h"

char coordinatorAddressString[17] = "";
uint8_t powerState = 0;

void setup() {
  // Define used pin states and put everything else high
  setPinModes();
  // start serials and Wire
  Wire.begin();
  SerialMon.begin(115200);
  SerialAT.begin(115200);
  delay(3000);
  // Init SerialMon flash
  initFlash();
  // Get id
  readIdFromEepRom(coordinatorAddressString);
  // Delay startup to allow programming
  delayStartup();
  // Display information to SerialMon
  displayCoordinatorData(coordinatorAddressString);  
  // init communications
  powerState = gprsPowerOn(powerState);
  mqttInit(coordinatorAddressString);
  mqttRegister(coordinatorAddressString);
  //powerState = gprsPowerOff(powerState);
  // Init sensors
  initSensors();
  delay(2000); // let all initialisations run out
}

void loop() {
  SerialMon.println(":: Main : Loop");
  digitalWrite(LED_BUILTIN, HIGH);
  LocalData_t localData;

  // set new alarm
  setRtcAlarm(SLEEPTIMER);
  // collect
  getCoordinatorData(&localData);
  getWeatherData(&localData);
  getScaleData(&localData);
  showLocalData(&localData);
  // send
  powerState = gprsPowerOn(powerState);
  mqttSendData(&localData);
  powerState = gprsPowerOff(powerState);
  // sleep
  #ifdef DEBUG
  SerialMon.println(":: Wait");
    digitalWrite(LED_BUILTIN, LOW);
    delay(DELAY_TIMER);
  #else
    setRtcAlarm(15);
    sleepCoordinator();
  #endif
}
