#ifndef GPRS_H   /* Include guard */
#define GPRS_H

#include <arduino.h>
#include "pins.h"

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <PubSubClient.h>

  // Communications
  void gprsTest();
  void gprsResetModem();
  void gprsConnectNetwork();
  void gprsEnd();
  void gprsPowerOn();
  void gprsPowerOff();  
  // MQTT
  void mqttInit(char *coordinatorAddressString);
  void mqttRegister();
  void mqttSendData(LocalData_t *local);

#endif