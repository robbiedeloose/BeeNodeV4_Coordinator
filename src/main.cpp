////////////////////////////////////////////////////////////////////////////////
// BEELOG V4 
////////////////////////////////////////////////////////////////////////////////
///////////////////////// OPTIONS //////////////////////////////////////////////
  #define SLEEPTIMER 1      // how often do we want to send data (in minutes)
  #define STARTDELAY 20      // delay start of programm, needed for reprogamming when using sleep
  #define DEBUG             // comment to use sleep
  #define DELAY_TIMER 2000  
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

  //#define dtrPin 11
  #define gsmResetPin 11

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
    #define CHANNEL_COUNT (int)(sizeof(DOUTS)/sizeof(byte))
    long int results[CHANNEL_COUNT];
    HX711MULTI scales(CHANNEL_COUNT, DOUTS, scaleClock);

/******************************* STRUCTS **************************************/
  ///////////////////////////////////// STRUCTS & STRUCT ARAYS ///////////////////
  struct LocalData_t {
    int16_t baseTemp;
    uint16_t baseHum;
    uint16_t baseLux;
    uint16_t baseBat;
    long int weights[CHANNEL_COUNT];
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
  void getScaleData(LocalData_t *local, int nbOfReads = 10, int maxDeviation = 1000);
  void showLocalData(LocalData_t *local);
  // Communications
  void gprsTest();
  void gprsResetModem();
  void gprsConnectNetwork();
  void gprsEnd();
  void gprsSleep();
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
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(3000);
  // Init serial flash
  initFlash();
  // Get id
  readIdFromEepRom();
    // Delay startup to allow programming
  delayStartup();
  // Display information to serial
  displayCoordinatorData();  

  // init communications
  mqttInit();
  // Init sensors
  myHumidity.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
}

/////////////// LOOP ///////////////////////////////////////////////////////////
void loop() {
    Serial.println(":: Loop");
    digitalWrite(LED_BUILTIN, HIGH);
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
  pinMode(gsmResetPin, OUTPUT);
  digitalWrite(gsmResetPin, HIGH);
}

void readIdFromEepRom() {
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

void initFlash() {
  SerialFlash.begin(flashChipSelect);
  SerialFlash.sleep(); 
}

void displayCoordinatorData() {
  Serial.println(F("BeeNode v4.0.1a"));
  char buf[17] = "";
  sprintf(buf, "%02X%02X%02X%02X%02X%02X%02X%02X", coordinatorAddress[0], coordinatorAddress[1], coordinatorAddress[2], coordinatorAddress[3], coordinatorAddress[4], coordinatorAddress[5], coordinatorAddress[6], coordinatorAddress[7]);
  Serial.print("Id: ");
  Serial.println(buf);
} 

void delayStartup() {
   /***** IMPORTANT DELAY FOR CODE UPLOAD BEFORE USB PORT DETACH DURING SLEEP *****/
  for(uint8_t i = 0; i < (STARTDELAY*2)+1; i++) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.print(".");
    delay(500);
  }
  Serial.println();
}

/******************************* RTC + SLEEP **********************************/
void setRtcAlarm(uint8_t alarmMinutes) {
  Serial.println(":: setRtcAlarm");
  rtc.setAlarmSeconds(0);
  rtc.setAlarmMinutes((rtc.getMinutes()+alarmMinutes)%60);
  rtc.enableAlarm(rtc.MATCH_MMSS);
  rtc.attachInterrupt(alarmMatch);
}

void sleepCoordinator() {
  Serial.println(F("sleep"));
  digitalWrite(LED_BUILTIN, LOW);
  delay(500); // give serial time to complete before node goes to sleep
}

void alarmMatch() {}

/******************************* sensors **************************************/
void getCoordinatorData(LocalData_t *local) {
  Serial.println(":: getCoordinatorData");
  // voltage / id
  local->baseBat = analogRead(A5)*4.3;
}

void getWeatherData(LocalData_t *local) {
  Serial.println(":: getWeatherData");
  local->baseTemp = myHumidity.readTemperature() * 100;
  local->baseHum = myHumidity.readHumidity() * 100;
  local->baseLux = lightMeter.readLightLevel();
}

void getScaleData(LocalData_t *local, int nbOfReads, int maxDeviation) {
  Serial.println(":: getScaleData");
  long int weights[nbOfReads][CHANNEL_COUNT];
  for (int read = 0; read < nbOfReads; read++) {
    scales.read(weights[read]);
    delay(500);
  }
  for (int channel = 0; channel < CHANNEL_COUNT; channel++) {
    int sum = 0;
    float average = 0.0;
    for (int read = 0; read < nbOfReads; read++) {
      int nextValue = weights[read][channel];
      if ((read = 0) or (abs(nextValue - average) < maxDeviation)) {
        sum += nextValue;
      } else {
        sum += int(floor(average));
      }
      average = sum / (read + 1);
    }
    local->weights[channel] = int(floor(sum / nbOfReads));
  }
}

void showLocalData(LocalData_t *local) {
  Serial.println(":: showLocalData");
  Serial.print("Temperature: ");
  Serial.println(local->baseTemp);
  Serial.print("Humidity: ");
  Serial.println(local->baseHum);
  Serial.print("Lux: ");
  Serial.println(local->baseLux);
  Serial.print("Bat: ");
  Serial.println(local->baseBat);
  Serial.print("Scale1: ");
  Serial.println(local->weights[0]);
  Serial.print("Scale2: ");
  Serial.println(local->weights[1]);
  Serial.print("Scale3: ");
  Serial.println(local->weights[2]);
  Serial.print("Scale4: ");
  Serial.println(local->weights[3]);
  Serial.print("Scale5: ");
  Serial.println(local->weights[4]);
  Serial.print("Scale6: ");
  Serial.println(local->weights[5]);
}

/******************************* Communication ********************************/
//////////// MQTT Code /////////////////////////////////////////////////////////
void mqttInit() {
  Serial.println(":: mqttInit");
  mqtt.setServer(broker, mqttPort);
  for(size_t i = 0; i < 17; i++) {
    mqttClient[i]=coordinatorAddressString[i];
  }
  mqttClient[16] = '\0';
  //mqtt.setCallback(mqttCallback);
  Serial.print("Mqtt Client ID: ");
  Serial.println(mqttClient);
  mqttRegister();
}

void mqttRegister() {
  Serial.println(":: mqttRegsister");
  gprsResetModem();
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    mqtt.publish("c/r", mqttClient);
  }
  mqtt.disconnect();
  gprsEnd();
  Serial.println("Registered CO");
  //gprsSleep();
}

void mqttSendData(LocalData_t *local) {
  Serial.println(":: mqttSendData");
  gprsResetModem();
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    char buf[120] = "";
    sprintf(buf, "%s,%i,%u,%d,%u", mqttClient, local->baseTemp, local->baseHum, local->baseLux, local->baseBat);
    Serial.println(buf);
    mqtt.publish("c/d", buf);
    sprintf(buf, "%s,%i,%li,%li,%li,%li,%li,%li,%li,%li,%li", mqttClient, local->baseTemp, local->weights[0], local->weights[1], local->weights[2], local->weights[3], local->weights[4], local->weights[5], local->weights[6], local->weights[7], local->weights[8]);
    Serial.println(buf);
    mqtt.publish("c/s", buf);
  }  
  mqtt.disconnect();
  gprsEnd();
  delay(250);
  gprsSleep();
}

//////////// init gprs, connect and disconnect from network ////////////////////
void gprsTest() {
  gprsResetModem();
  gprsConnectNetwork();
  gprsEnd();
}

void gprsResetModem() {
  Serial.println(":: gprsResetModem");
  digitalWrite(gsmResetPin, LOW);
  digitalWrite(gsmResetPin, HIGH);
  delay(100);
  //modem.restart();
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

void gprsSleep() {
  Serial.println(":: gprsSleep");
  modem.poweroff();
}
