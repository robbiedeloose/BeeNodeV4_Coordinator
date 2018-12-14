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
  SerialMon.print(F("Mqtt Client ID: "));
  SerialMon.println(mqttClient);
}

void mqttRegister(char* coordinatorAddressString) {
  SerialMon.println(F(":: mqttRegsister"));
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    delay(100);
    mqtt.publish("c/r", mqttClient);
  }
  mqtt.disconnect();
  gprsEnd();
  SerialMon.println(F(" -- Registered CO"));
}

void mqttSendData(LocalData_t *local, HiveData_t *hive) {
  SerialMon.println(F(":: mqttSendData"));
  gprsConnectNetwork();
  if (mqtt.connect(mqttClient, mqttUser, mqttPswd)) {
    char buf[120] = "";
    SerialMon.println(F("Sending co Data"));
    sprintf(buf, "%s,%i,%u,%d,%u", mqttClient, local->baseTemp, local->baseHum, local->baseLux, local->baseBat);
    SerialMon.println(buf);
    mqtt.publish("c/d", buf);
    SerialMon.println(F("Sending hive Data"));
    for(size_t i = 0; i < HIVE_BUFFER; i++)
    {
      if (hive->bat[i] != 0) {
        sprintf(buf, "%s,%i,%u,%d,%u,%s,%i,%u,%u,%u", mqttClient, local->baseTemp, local->baseHum, local->baseLux, local->baseBat, hive->id[i], hive->temp[i], hive->hum[i], hive->bat[i], hive->weight[i] );
        SerialMon.println(buf);
        mqtt.publish("c/h", buf);
      }
    }
    SerialMon.println(F("Sending scale Data"));
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
  SerialMon.println(F(":: gprsResetModem"));
  modem.restart();
  String modemInfo = modem.getModemInfo();
  SerialMon.print(F(" Modem: "));
  SerialMon.println(modemInfo);
}

void gprsDisplayModemInfo() {
  SerialMon.println(F(":: gprsResetModem"));
  modem.restart();
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
  SerialMon.println(F(" Disconnected"));
}

void gprsPowerOn() {
  digitalWrite(GSM_RESET_PIN, HIGH);
}

void gprsPowerOff() {
  digitalWrite(GSM_RESET_PIN, LOW);
}
