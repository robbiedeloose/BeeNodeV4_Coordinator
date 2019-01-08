#ifndef GPRS_H   /* Include guard */
#define GPRS_H

#include <arduino.h>
#include "config.h"

void gprsTest();
void gprsResetModem();
void gprsConnectNetwork();
void gprsEnd();
void gprsPushPowerButton(unsigned long milliseconds);
void gprsPowerOn();
void gprsPowerOff();

void mqttInit(char* coordinatorAddressString);
void mqttRegister(char* coordinatorAddressString);
void mqttSendData(LocalData_t *local, HiveData_t *hive);

#endif
