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
bool sleepEnabled = false;

void setup() {
  // Define used pin states and put everything else high
  setPinModes();
  // start serials and Wire
  Wire.begin();
  SerialMon.begin(115200);
  SerialAT.begin(115200);
  //delay(3000);
  // Init SerialMon flash
  initFlash();
  // Get id
  readIdFromEepRom(coordinatorAddressString);
  // Delay startup to allow programming
  delayStartup();
  // Display information to SerialMon
  digitalWrite(LED_BUILTIN, HIGH);
  displayCoordinatorData(coordinatorAddressString);  
  
  if (digitalRead(SLEEP_ENABLED) == LOW) {
    sleepEnabled = true;
    SerialMon.println(":::: Sleep Enabled");
  } else {
    sleepEnabled = false;
    SerialMon.println(":::: Sleep Disabled");
  }

  //init RTC
  initRtc();
  // init communications
  powerState = gprsPowerOn(powerState);
  mqttInit(coordinatorAddressString);
  mqttRegister(coordinatorAddressString);
  powerState = gprsPowerOff(powerState);
  delay(1000);

  // Init sensors
  initSensors();
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000); // let all initialisations run out
}

void loop() {
  SerialMon.println(":: Loop");
  digitalWrite(LED_BUILTIN, HIGH);
  LocalData_t localData;

  // set new alarm
  setRtcAlarm(SLEEPTIMER); 
  // collect
  getCoordinatorData(&localData);
  getWeatherData(&localData);
  getScaleData(&localData, SCALE_SAMPLE_RATE);
  showLocalData(&localData);
  // send
  powerState = gprsPowerOn(powerState);
  mqttSendData(&localData);
  powerState = gprsPowerOff(powerState);
  // sleep
  if (sleepEnabled) {
    SerialMon.println(":: Sleep");
    for(size_t i = 0; i < 10; i++)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(100);
    }
    digitalWrite(LED_BUILTIN, LOW);
    sleepCoordinator();
  } else {
    SerialMon.println(":: Wait");
    digitalWrite(LED_BUILTIN, LOW);
    delay(DELAY_TIMER);
  }
}
