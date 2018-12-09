#ifndef READID_H   /* Include guard */
#define READID_H

#include <arduino.h>
#include <wire.h>

void readIdFromEepRom(char* coordinatorAddressString);

#endif