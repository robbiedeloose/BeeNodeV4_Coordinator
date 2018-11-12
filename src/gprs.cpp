#include "gprs.h"

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

/* MQTT */
void mqttInit(char* coordinatorAddressString) {
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

void mqttRegister(char* coordinatorAddressString) {
  SerialMon.println(":: mqttRegsister");
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    delay(100);
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
    sprintf(buf, "%s,%i,%li,%li,%li,%li,%li,%li", mqttClient, local->baseTemp, local->weights[0], local->weights[1], local->weights[2], local->weights[3], local->weights[4], local->weights[5]);
    SerialMon.println(buf);
    mqtt.publish("c/s", buf);
  }  
  mqtt.disconnect();
  gprsEnd();
  delay(250); /// check if this can be removed
}

/* GPRS */
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

void gprsPushPowerButton(unsigned long milliseconds) {  
  digitalWrite(GSM_RESET_PIN, LOW);
  delay(milliseconds); // should replace this with a 1,5s sleep
  digitalWrite(GSM_RESET_PIN, HIGH);
}

uint8_t gprsPowerOn(uint8_t powerstate) {
  SerialMon.println(":: gprsPowerOn");
  if (powerstate == 0) {
    gprsPushPowerButton(1500);
  }
  delay(2000);
  return 1;
}

uint8_t gprsPowerOff(uint8_t powerstate) {
  SerialMon.println(":: gprsPowerOff");
  if (powerstate == 1) {
    gprsPushPowerButton(1500);
  }
  return 0;
}
