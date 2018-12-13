#include "reciever.h"

void getDataFromReciever(HiveData_t *hive){
  SerialMon.println(":: getDataFromReciever");
  // Wake reciever
  digitalWrite(WAKE_RECIEVER, LOW);
  delay(100);
  digitalWrite(WAKE_RECIEVER, HIGH);
  SerialMon.print("wating for response");
  
  for(size_t i = 0; i < 5; i++)
  {
    delay(1000);
    SerialMon.print("."); 
  }
  SerialMon.println();

  // Make 6 requests and capture string
  for(size_t i = 0; i < HIVE_BUFFER; i++)
  {
    Wire.requestFrom(8, 28);    // request 28 bytes from slave device #8
  
    char buf[29];
    uint8_t pos = 0;
    while (Wire.available()) { // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      buf[pos] = c;
      pos++;
    }
    buf[28] = '\0';

    // Parse each request if there is data in the battery section
    if(buf[18] != '0') {
      SerialMon.println(buf);

      char hive_id[9];
      int hive_temp = 0;
      int hive_hum = 0;
      int hive_bat = 0;
      int hive_weight = 0;
      
      sscanf(buf, "%8s,%4d,%3d,%3d,%6d", hive_id, &hive_temp, &hive_hum, &hive_bat, &hive_weight );

      if (hive_bat != 0) {
        for(size_t a = 0; a < 8; a++) {
          hive->id[i][a] = hive_id[a];
        }
        hive->id[i][8] = '\0';
        hive->temp[i] = hive_temp;
        hive->hum[i] = hive_hum;
        hive->bat[i] = hive_bat;
        hive->weight[i] = hive_weight;
      }
    }
  }
}

void displayHiveBuffer(HiveData_t *hive) {
  SerialMon.println(":: displayHiveBuffer");
  for(size_t i = 0; i < HIVE_BUFFER; i++)
  {
    if (hive->bat[i] != 0) {
      SerialMon.print("Buffer location ");
      SerialMon.println(i);
      SerialMon.print("- id: ");
      SerialMon.println(hive->id[i]);
      SerialMon.print("- temp: ");
      SerialMon.println(hive->temp[i]);
      SerialMon.print("- hum: ");
      SerialMon.println(hive->hum[i]);
      SerialMon.print("- bat: ");
      SerialMon.println(hive->bat[i]);
      SerialMon.print("- weight: ");
      SerialMon.println(hive->weight[i]);
    }
  }
}

void clearHiveBuffer(HiveData_t *hive){
  for(size_t i = 0; i < HIVE_BUFFER; i++)
  {
    for(size_t a = 0; a < 8; a++)
    {
      hive->id[i][a] = 0;
    }
    hive->temp[i] = 0;
    hive->hum[i] = 0;
    hive->bat[i] = 0;
    hive->weight[i] = 0;
  }
}
