#ifndef READID_H   /* Include guard */
#define READID_H

#include <arduino.h>
#include <wire.h>
#include "config.h"

void readIdFromEepRom(char* coordinatorAddressString);

#endif