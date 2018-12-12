#ifndef RECIEVER_H   /* Include guard */
#define RECIEVER_H

#include <arduino.h>
#include <wire.h>
#include "config.h"

void getDataFromReciever(HiveData_t *hive);
void displayHiveBuffer(HiveData_t *hive);
void clearHiveBuffer(HiveData_t *hive);

#endif