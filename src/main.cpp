


///////////////////////// OPTIONS //////////////////////////////////////////////
#define SLEEPTIMER 1      // how often do we want to send data (in minutes)
#define DEBUG             // comment to use sleep
///////////////////////// PIN DEFINES //////////////////////////////////////////
#define flashChipCSPin 4
#define buildInLed 13

///////////////////////// General Stuff ////////////////////////////////////////
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

/******************************* BOARD FUNCTIONS ******************************/
  #define Serial SerialUSB 
  ////////////////////////////////// SERIAL FLASH ////////////////////////////////
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
  uint8_t coordinatorAddress[8];

/******************************* Communication ********************************/
  ///////////////////////////////////////// GPRS /////////////////////////////////
  #define TINY_GSM_MODEM_SIM800
  #include <PubSubClient.h>
  #include <TinyGsmClient.h>
  // Your GPRS credentials
  const char apn[] = "hologram";
  const char user[] = "";
  const char pass[] = "";
  // Server details
  const char *broker = "m20.cloudmqtt.com";
  const uint16_t mqttPort = 15913;
  const char *mqttUser = "oseiokpx";
  const char *mqttPswd = "31IMHCdWxVVt";

  TinyGsm modem(Serial1);
  TinyGsmClient client(modem);
  PubSubClient mqtt(client);
  ///////////////////////////////////// MQTT //////////////////////////////////////
  char mqttClient[12] = "";

/******************************* Sensors **************************************/
  //////////////////////////////// HUMIDITY //////////////////////////////////////
  #include "SparkFunHTU21D.h"
  HTU21D myHumidity;
  //////////////////////////////// LUX ///////////////////////////////////////////
  #include <BH1750.h>
  BH1750 lightMeter;
  //////////////////////////////// Scale /////////////////////////////////////////
  #include "HX711-multi.h"
  // Pins to the load cell amp
  #define CLK 2      // clock pin to the load cell amp
  #define DOUT1 3    // data pin to the first lca
  #define DOUT2 4    // data pin to the second lca
  #define DOUT3 5    // data pin to the third lca
  byte DOUTS[3] = {DOUT1, DOUT2, DOUT3};
  #define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)
  long int results[CHANNEL_COUNT];
  HX711MULTI scales(CHANNEL_COUNT, DOUTS, CLK);

/******************************* STRUCTS **************************************/
  ///////////////////////////////////// STRUCTS & STRUCT ARAYS ///////////////////
  struct LocalData_t {
    int16_t baseTemp;
    uint16_t baseHum;
    uint16_t baseLux;
    uint16_t baseBat;
    char scales[2][23];
  };

/******************************* FUNCTION DECLARATIONS ************************/
  ////////////////////////// FUNCTION DECLARATIONS ///////////////////////////////
  // something
  void setPinModes();
  void readIdFromEepRom();
  void initFlash();
  void displayCoordinatorData();
  void delayStartup();
  void setRtcAlarm(uint8_t alarmMinutes);
  void sleepCoordinator();
  void alarmMatch();
  // sensors
  void getCoordinatorData(LocalData_t *local);
  void getWeatherData(LocalData_t *local);
  void getScaleData(LocalData_t *local);
  // communications
  void gprsResetModem();
  void gprsConnectNetwork();
  void gprsEnd();
  // MQTT
  void mqttRegister();
  void mqttSendData(LocalData_t *local);

/////////////// SETUP //////////////////////////////////////////////////////////
void setup() {
  // define used pin states and put everything else high
  setPinModes();
  delay(1000);
  // delay startup to allow programming
  delayStartup();
  delay(30000);

  // init serials
  //Serial.begin(9600);
  //Serial1.begin(9600);
 
  Wire.begin();
  // init serial flash
  initFlash();
  // init sensors
  myHumidity.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  // get id
  readIdFromEepRom();
  // init communications
  mqtt.setServer(broker, mqttPort);
  mqttRegister();
  // display information to serial
  displayCoordinatorData();
}

/////////////// LOOP ///////////////////////////////////////////////////////////
void loop() {
    LocalData_t localData;

    // set new alarm
    setRtcAlarm(SLEEPTIMER);
    // collect
    getCoordinatorData(&localData);
    getWeatherData(&localData);
    getScaleData(&localData);
    // send
    mqttSendData(&localData);
    // sleep
    #ifdef DEBUG
      delay(10000);
    #else
      sleepCoordinator();
    #endif

}

/******************************* BOARD SPECIFIC *******************************/

void setPinModes(){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void readIdFromEepRom(){
  //0x50 is the I2c address, 0xF8 is the memory address where the read-only MAC value is
  Wire.beginTransmission(0x50);
  Wire.write(0xF8); // LSB 
  Wire.endTransmission();
  Wire.requestFrom(0x50, 8); //request 8 bytes from the device
  uint8_t i = 0;
  while (Wire.available()){ 
    coordinatorAddress[i] = Wire.read();
    i++;
  }    
}

void initFlash(){
    SerialFlash.begin(flashChipSelect);
    SerialFlash.sleep(); 
}

void displayCoordinatorData(){
  Serial.println(F("BeeNode v4"));
  char buf[17] = "";
  sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X%02X", coordinatorAddress[0], coordinatorAddress[1], coordinatorAddress[2], coordinatorAddress[3], coordinatorAddress[4], coordinatorAddress[5], coordinatorAddress[6], coordinatorAddress[7]);
  Serial.print(" Id: ");
  Serial.println(buf);
} 

void delayStartup(){
   /***** IMPORTANT DELAY FOR CODE UPLOAD BEFORE USB PORT DETACH DURING SLEEP *****/
  uint8_t i = 0;
  while(i < 51){
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      i++;   
  }
}
/******************************* RTC + SLEEP **********************************/
void setRtcAlarm(uint8_t alarmMinutes){
    rtc.setAlarmSeconds(0);
    rtc.setAlarmMinutes((rtc.getMinutes()+alarmMinutes)%60);
    rtc.enableAlarm(rtc.MATCH_MMSS);
    rtc.attachInterrupt(alarmMatch);
}

void sleepCoordinator(){
    Serial.println(F("sleep"));
    delay(500); // give serial time to complete before node goes to sleep
}

void alarmMatch(){}

/******************************* sensors **************************************/
void getCoordinatorData(LocalData_t *local){
  // voltage / id
  local->baseBat = 0;
}

void getWeatherData(LocalData_t *local) {
  local->baseTemp = myHumidity.readTemperature() * 100;
  local->baseHum = myHumidity.readHumidity() * 100;
  local->baseLux = lightMeter.readLightLevel();
  
}

void getScaleData(LocalData_t *local) {
  // todo
}

/******************************* Communication ********************************/
//////////// MQTT Code /////////////////////////////////////////////////////////
void mqttRegister() {
  gprsResetModem();
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    mqtt.publish("c/r", mqttClient);
  }
  gprsEnd();
}

void mqttSendData(LocalData_t *local) {
  gprsResetModem();
  gprsConnectNetwork();
  /*for (int b = 0; b < BUFFERSIZE; b++) {
    if (plBuffer[b].temp[0] != 0) {
      Serial.print(F("Send buffer "));
      Serial.println(b);
      if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
        char buf[120] = "";
        sprintf(buf, "%02X%02X%02X%02X,%d,%d,%d,%d,%d,%d,%u,%u,%u,%u,%d", plBuffer[b].id[0], plBuffer[b].id[1], plBuffer[b].id[2], plBuffer[b].id[3], plBuffer[b].temp[0], plBuffer[b].temp[1], plBuffer[b].temp[2], plBuffer[b].temp[3], plBuffer[b].temp[4], plBuffer[b].temp[5],plBuffer[b].humidity,plBuffer[b].bat,local->baseTemp,local->baseHum,local->baseLux);
        mqtt.publish("h/d", buf);
      }
    }
  }*/
  mqtt.disconnect();
  gprsEnd();
}

//////////// init gprs, connect and disconnect from network ////////////////////
void gprsResetModem() {
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print(F(" Modem: "));
  Serial.println(modemInfo);
}

void gprsConnectNetwork() {
  Serial.print(F(" Waiting for network..."));
  if (!modem.waitForNetwork()) {
    Serial.println(F(" fail"));
    delay(10000);
    return;
  }
  Serial.println(F(" OK"));
  Serial.print(F(" Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(F(" fail"));
    delay(10000);
    return;
  }
  Serial.println(F(" OK"));
}

void gprsEnd() {
  modem.gprsDisconnect();
}
