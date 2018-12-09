#include "flash.h"

void initFlash() {
  SerialMon.print("Init Flash ");
  SerialFlash.begin(flashChipSelect);
  SerialFlash.sleep();
  SerialMon.println("- done"); 
}
