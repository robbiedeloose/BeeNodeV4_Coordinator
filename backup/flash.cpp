#include "flash.h"

void initFlash() {
  SerialFlash.begin(flashChipSelect);
  SerialFlash.sleep(); 
}
