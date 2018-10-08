


///////////////////////// OPTIONS //////////////////////////////////////////////
#define SLEEPTIMER 1      // how often do we want to send data (in minutes)
#define STARTDELAY 5      // delay start of programm, needed for reprogamming when using sleep
#define DEBUG             // comment to use sleep
///////////////////////// PIN DEFINES //////////////////////////////////////////
#define flashChipCSPin 4
#define buildInLed 13

#define scaleClock 15
#define scaleData1 5
#define scaleData2 6
#define scaleData3 7
#define scaleData4 16
#define scaleData5 17
#define scaleData6 18
#define scaleData7 8
#define scaleData8 9
#define scaleData9 10

#define dtrPin 11

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

  TinyGsm modem(Serial1);
  TinyGsmClient client(modem);
  PubSubClient mqtt(client);
  char mqttClient[17] = "";

/******************************* Sensors **************************************/
  //////////////////////////////// HUMIDITY //////////////////////////////////////
  #include "SparkFunHTU21D.h"
  HTU21D myHumidity;
  //////////////////////////////// LUX ///////////////////////////////////////////
  #include <BH1750.h>
  BH1750 lightMeter;

  ////////////////////////////////// SCALES ////////////////////////////////////////
    #include "HX711-multi.h"
    byte DOUTS[9] = {scaleData1, scaleData2, scaleData3, scaleData4, scaleData5, scaleData6, scaleData7, scaleData8, scaleData9};
    #define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)
    long int results[CHANNEL_COUNT];
    HX711MULTI scales(CHANNEL_COUNT, DOUTS, scaleClock);

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
  // Something
  void setPinModes();
  void readIdFromEepRom();
  void initFlash();
  void displayCoordinatorData();
  void delayStartup();
  void setRtcAlarm(uint8_t alarmMinutes);
  void sleepCoordinator();
  void alarmMatch();
  // Sensors
  void getCoordinatorData(LocalData_t *local);
  void getWeatherData(LocalData_t *local);
  void getScaleData(LocalData_t *local);
  void showLocalData(LocalData_t *local);
  // Communications
  void gprsTest();
  void gprsResetModem();
  void gprsConnectNetwork();
  void gprsEnd();
  // MQTT
  void initMqtt();
  void mqttRegister();
  //void mqttSendData(LocalData_t *local);

/******************************************************************************/

/////////////// SETUP //////////////////////////////////////////////////////////
void setup() {
  // Define used pin states and put everything else high
  setPinModes();
  // start serials 
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(3000);
  // Delay startup to allow programming
  delayStartup();
  Wire.begin();
  // Init serial flash
  initFlash();
  // Get id
  readIdFromEepRom();
  // init communications
  initMqtt();
  // Init sensors
  myHumidity.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  // Display information to serial
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
    // show
    showLocalData(&localData);
    // send
    //mqttSendData(&localData);
    // sleep
    #ifdef DEBUG
      delay(2000);
    #else
      sleepCoordinator();
    #endif

}

/******************************* BOARD SPECIFIC *******************************/
void setPinModes(){
  pinMode(A5,INPUT);
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
  sprintf(coordinatorAddressString, "%02X%02X%02X%02X%02X%02X%02X%02X", coordinatorAddress[0], coordinatorAddress[1], coordinatorAddress[2], coordinatorAddress[3], coordinatorAddress[4], coordinatorAddress[5], coordinatorAddress[6], coordinatorAddress[7]);
  
}

void initFlash(){
    SerialFlash.begin(flashChipSelect);
    SerialFlash.sleep(); 
}

void displayCoordinatorData(){
  Serial.println(F("BeeNode v4.0.1a"));
  char buf[17] = "";
  sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X%02X", coordinatorAddress[0], coordinatorAddress[1], coordinatorAddress[2], coordinatorAddress[3], coordinatorAddress[4], coordinatorAddress[5], coordinatorAddress[6], coordinatorAddress[7]);
  Serial.print("Id: ");
  Serial.println(buf);
} 

void delayStartup(){
   /***** IMPORTANT DELAY FOR CODE UPLOAD BEFORE USB PORT DETACH DURING SLEEP *****/

  for(uint8_t i = 0; i < (STARTDELAY*2); i++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.print(".");
    delay(500);
  }
  Serial.println();
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
  
  int sensorValue = analogRead(A5);
  float voltage= sensorValue * (5.0 / 1023.0);

  local->baseBat = analogRead(A5)*4.3;
}

void getWeatherData(LocalData_t *local) {
  local->baseTemp = myHumidity.readTemperature() * 100;
  local->baseHum = myHumidity.readHumidity() * 100;
  local->baseLux = lightMeter.readLightLevel();
}

void getScaleData(LocalData_t *local) {
    scales.read(results);
}

void showLocalData(LocalData_t *local){
  Serial.print("Temperature: ");
  Serial.println(local->baseTemp);
  Serial.print("Humidity: ");
  Serial.println(local->baseHum);
  Serial.print("Lux: ");
  Serial.println(local->baseLux);
  Serial.print("Bat: ");
  Serial.println(local->baseBat);
  Serial.print("Scale1: ");
  Serial.println(results[0]);
  Serial.print("Scale2: ");
  Serial.println(results[1]);
  Serial.print("Scale3: ");
  Serial.println(results[2]);
  Serial.print("Scale4: ");
  Serial.println(results[3]);
  Serial.print("Scale5: ");
  Serial.println(results[4]);
  Serial.print("Scale6: ");
  Serial.println(results[5]);
}

/******************************* Communication ********************************/
//////////// MQTT Code /////////////////////////////////////////////////////////
void initMqtt(){
  mqtt.setServer(broker, mqttPort);
  for(size_t i = 0; i < 17; i++)
  {
    mqttClient[i]=coordinatorAddressString[i];
  }
  mqttClient[16] = '\0';
  //mqtt.setCallback(mqttCallback);
  Serial.print("Mqtt Client ID: ");
  Serial.println(mqttClient);
  mqttRegister();
}

void mqttRegister() {
  gprsResetModem();
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    mqtt.publish("c/r", mqttClient);
  }
  mqtt.disconnect();
  gprsEnd();
  Serial.println("Registered CO");
}

void mqttSendData(LocalData_t *local) {
  gprsResetModem();
  gprsConnectNetwork();
      if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
        //char buf[120] = "";
        //sprintf(buf, "%02X%02X%02X%02X,%d,%d,%d,%d,%d,%d,%u,%u,%u,%u,%d", plBuffer[b].id[0], plBuffer[b].id[1], plBuffer[b].id[2], plBuffer[b].id[3], plBuffer[b].temp[0], plBuffer[b].temp[1], plBuffer[b].temp[2], plBuffer[b].temp[3], plBuffer[b].temp[4], plBuffer[b].temp[5],plBuffer[b].humidity,plBuffer[b].bat,local->baseTemp,local->baseHum,local->baseLux);
        //mqtt.publish("c/d", buf);
      }  
  mqtt.disconnect();
  gprsEnd();
}
//////////// init gprs, connect and disconnect from network ////////////////////
void gprsTest() {
  gprsResetModem();
  gprsConnectNetwork();
  gprsEnd();
}

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
  Serial.println(" Disconnected");
}

void gprsSleep(){
  modem.sleepEnable();
}