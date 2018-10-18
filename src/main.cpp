////////////////////////////////////////////////////////////////////////////////
// BEELOG V4 
////////////////////////////////////////////////////////////////////////////////
///////////////////////// OPTIONS //////////////////////////////////////////////
  #define SLEEPTIMER 1      // how often do we want to send data (in minutes)
  #define STARTDELAY 20      // delay start of programm, needed for reprogamming when using sleep
  #define DEBUG             // comment to use sleep
  #define DELAY_TIMER 10000  
///////////////////////// PIN DEFINES //////////////////////////////////////////
  #define flashChipCSPin 4
  #define buildInLed 13

  #define GSM_RESET_PIN 2

///////////////////////// General Stuff ////////////////////////////////////////
  #include <Arduino.h>
  #include <Wire.h>
  #include <SPI.h>

  // test includes for splitting to multiple files
  #include "gprsPower.h"
  #include "readId.h"
  #include "sensors.h"

/******************************* BOARD FUNCTIONS ******************************/
  #define SerialMon SerialUSB
  #define SerialAT Serial
  ////////////////////////////////// SerialMon FLASH ////////////////////////////////
  #include <SerialFlash.h>
  const int flashChipSelect = flashChipCSPin;
  ////////////////////////////////// RTC + SLEEP /////////////////////////////////
  #include <RTCZero.h>
  RTCZero rtc;
  /* Change these values to set the current initial time */
  const uint8_t seconds = 0;
  const uint8_t minutes = 57;
  const uint8_t hours = 10;
  /* Change these values to set the current initial date */
  const uint8_t day = 20;
  const uint8_t month = 2;
  const uint8_t year = 17;
  ///////////////////////////////////// ID ///////////////////////////////////////
  char coordinatorAddressString[17] = "";

/******************************* Communication ********************************/
  #define TINY_GSM_MODEM_SIM800
  #include <TinyGsmClient.h>
  #include <PubSubClient.h>
  // Your GPRS credentials
  const char apn[] = "hologram";
  const char user[] = "";
  const char pass[] = "";
  // Server details
  const char *broker = "m20.cloudmqtt.com";
  const uint16_t mqttPort = 15913;
  const char *mqttUser = "oseiokpx";
  const char *mqttPswd = "31IMHCdWxVVt";

  TinyGsm modem(SerialAT);
  TinyGsmClient client(modem);
  PubSubClient mqtt(client);
  char mqttClient[17] = "";
  uint8_t powerState = 0;


/******************************* STRUCTS **************************************/


/******************************* FUNCTION DECLARATIONS ************************/
  ////////////////////////// FUNCTION DECLARATIONS ///////////////////////////////
  // Something
  void setPinModes();
  void initFlash();
  void displayCoordinatorData();
  void delayStartup();
  void setRtcAlarm(uint8_t alarmMinutes);
  void sleepCoordinator();
  void alarmMatch();
  // Sensors
  void getCoordinatorData(LocalData_t *local);
  void showLocalData(LocalData_t *local);
  // Communications
  void gprsTest();
  void gprsResetModem();
  void gprsConnectNetwork();
  void gprsEnd();
  void gprsPowerOn();
  void gprsPowerOff();  
  // MQTT
  void mqttInit();
  void mqttRegister();
  void mqttSendData(LocalData_t *local);

/******************************************************************************/

/////////////// SETUP //////////////////////////////////////////////////////////
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
  displayCoordinatorData();  
  // init communications
  gprsPowerOn(powerState);
  mqttInit();
  mqttRegister();
  gprsPowerOff(powerState);
  // Init sensors

  delay(2000); // let all initialisations run out
}

/////////////// LOOP ///////////////////////////////////////////////////////////
void loop() {
    SerialMon.println(":: Loop");
    digitalWrite(LED_BUILTIN, HIGH);
    LocalData_t localData;

    // set new alarm
    setRtcAlarm(SLEEPTIMER);
    // collect
    getCoordinatorData(&localData);
    getWeatherData(&localData);
    gprsPowerOn(powerState);
    getScaleData(&localData);
    // show
    showLocalData(&localData);
    // send
   
    mqttSendData(&localData);
    gprsPowerOff(powerState);
    // sleep
    #ifdef DEBUG
      digitalWrite(LED_BUILTIN, LOW);
      delay(DELAY_TIMER);
    #else
      sleepCoordinator();
    #endif
}

/******************************* BOARD SPECIFIC *******************************/
void setPinModes() {
  pinMode(A5,INPUT);
  //led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //gprssleep
  pinMode(GSM_RESET_PIN, OUTPUT);
  digitalWrite(GSM_RESET_PIN, HIGH);
}

void initFlash() {
  SerialFlash.begin(flashChipSelect);
  SerialFlash.sleep(); 
}

void displayCoordinatorData() {
  SerialMon.println(F("BeeNode v4.0.1a"));
  //char buf[17] = "";
  //sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X%02X", coordinatorAddress[0], coordinatorAddress[1], coordinatorAddress[2], coordinatorAddress[3], coordinatorAddress[4], coordinatorAddress[5], coordinatorAddress[6], coordinatorAddress[7]);
  SerialMon.print("Id: ");
  SerialMon.println(coordinatorAddressString);
} 

void delayStartup() {
   /***** IMPORTANT DELAY FOR CODE UPLOAD BEFORE USB PORT DETACH DURING SLEEP *****/
  for(uint8_t i = 0; i < (STARTDELAY*2)+1; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    SerialMon.print(".");
    delay(500);
  }
  SerialMon.println();
}

/******************************* RTC + SLEEP **********************************/
void setRtcAlarm(uint8_t alarmMinutes) {
  SerialMon.println(":: setRtcAlarm");
  rtc.setAlarmSeconds(0);
  rtc.setAlarmMinutes((rtc.getMinutes()+alarmMinutes)%60);
  rtc.enableAlarm(rtc.MATCH_MMSS);
  rtc.attachInterrupt(alarmMatch);
}

void sleepCoordinator() {
  SerialMon.println(F("sleep"));
  digitalWrite(LED_BUILTIN, LOW);
  delay(500); // give SerialMon time to complete before node goes to sleep
}

void alarmMatch() {}

/******************************* sensors **************************************/
void getCoordinatorData(LocalData_t *local) {
  SerialMon.println(":: getCoordinatorData");
  // voltage / id
  local->baseBat = analogRead(A5)*4.3;
}

void showLocalData(LocalData_t *local) {
  SerialMon.println(":: showLocalData");
  SerialMon.print("Temperature: ");
  SerialMon.println(local->baseTemp);
  SerialMon.print("Humidity: ");
  SerialMon.println(local->baseHum);
  SerialMon.print("Lux: ");
  SerialMon.println(local->baseLux);
  SerialMon.print("Bat: ");
  SerialMon.println(local->baseBat);
  SerialMon.print("Scale1: ");
  SerialMon.println(local->weights[0]);
  SerialMon.print("Scale2: ");
  SerialMon.println(local->weights[1]);
  SerialMon.print("Scale3: ");
  SerialMon.println(local->weights[2]);
  SerialMon.print("Scale4: ");
  SerialMon.println(local->weights[3]);
  SerialMon.print("Scale5: ");
  SerialMon.println(local->weights[4]);
  SerialMon.print("Scale6: ");
  SerialMon.println(local->weights[5]);
}

/******************************* Communication ********************************/
//////////// MQTT Code /////////////////////////////////////////////////////////
void mqttInit() {
  SerialMon.println(":: mqttInit");
  mqtt.setServer(broker, mqttPort);
  for(size_t i = 0; i < 17; i++) {
    mqttClient[i]=coordinatorAddressString[i];
  }
  mqttClient[16] = '\0';
  //mqtt.setCallback(mqttCallback); // we can use this to reply options to CO
  SerialMon.print("Mqtt Client ID: ");
  SerialMon.println(mqttClient);

}

void mqttRegister() {
  SerialMon.println(":: mqttRegsister");
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    mqtt.publish("c/r", mqttClient);
  }
  mqtt.disconnect();
  gprsEnd();
  SerialMon.println("Registered CO");
}

void mqttSendData(LocalData_t *local) {
  SerialMon.println(":: mqttSendData");
  //gprsResetModem();
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    char buf[120] = "";
    sprintf(buf, "%s,%i,%u,%d,%u", mqttClient, local->baseTemp, local->baseHum, local->baseLux, local->baseBat);
    SerialMon.println(buf);
    mqtt.publish("c/d", buf);
    sprintf(buf, "%s,%i,%li,%li,%li,%li,%li,%li,%li,%li,%li", mqttClient, local->baseTemp, local->weights[0], local->weights[1], local->weights[2], local->weights[3], local->weights[4], local->weights[5], local->weights[6], local->weights[7], local->weights[8]);
    SerialMon.println(buf);
    mqtt.publish("c/s", buf);
  }  
  mqtt.disconnect();
  gprsEnd();
  delay(250); /// check if this can be removed
}

//////////// init gprs, connect and disconnect from network ////////////////////
void gprsTest() {
  gprsResetModem();
  gprsConnectNetwork();
  gprsEnd();
}

void gprsResetModem() {
  SerialMon.println(":: gprsResetModem");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  SerialMon.print(F(" Modem: "));
  SerialMon.println(modemInfo);
}

void gprsDisplayModemInfo() {
  SerialMon.println(":: gprsResetModem");
  //modem.restart();
  String modemInfo = modem.getModemInfo();
  SerialMon.print(F(" Modem: "));
  SerialMon.println(modemInfo);
}

void gprsConnectNetwork() {
  SerialMon.print(F(" Waiting for network..."));
  if (!modem.waitForNetwork()) {
    SerialMon.println(F(" fail"));
    delay(10000);
    return;
  }
  SerialMon.println(F(" OK"));
  SerialMon.print(F(" Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(F(" fail"));
    delay(10000);
    return;
  }
  SerialMon.println(F(" OK"));
}

void gprsEnd() {
  modem.gprsDisconnect();
  //gprsPowerOff();
  SerialMon.println(" Disconnected");
}