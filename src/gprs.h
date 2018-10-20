#ifndef GPRS_H   /* Include guard */
#define GPRS_H

#include <arduino.h>
#include "pins.h"
#include "config.h"

  void gprsTest();
  void gprsResetModem();
  void gprsConnectNetwork();
  void gprsEnd();
  uint8_t gprsPowerOn(uint8_t powerstate);
  uint8_t gprsPowerOff(uint8_t powerstate);

  void mqttInit(char* coordinatorAddressString);
  void mqttRegister(char* coordinatorAddressString);
  void mqttSendData(LocalData_t *local);

#endif
