#include "reciever.h"

void getDataFromReciever(){
  SerialMon.println(":: getDataFromReciever");
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //SerialMon.println("low");
  digitalWrite(WAKE_RECIEVER, LOW);
  delay(100);
  //SerialMon.println("high");
  digitalWrite(WAKE_RECIEVER, HIGH);
  SerialMon.print("wating for responce");
  
  for(size_t i = 0; i < 5; i++)
  {
    delay(1000);
    SerialMon.print("."); 
  }
  SerialMon.println();

  //make 6 requests to get all plBuffer data
  for(size_t i = 0; i < 6; i++)
  {
    //SerialMon.print("request ");
    //SerialMon.println(i);
    Wire.requestFrom(8, 28);    // request 6 bytes from slave device #8
  
    char buf[29];
    uint8_t pos = 0;
    while (Wire.available()) { // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      buf[pos] = c;
      pos++;
    }
    buf[28] = '\0';
    if(buf[18] != '0') {
      SerialMon.println(buf);
      
      char hive_id[9];
      int hive_temp = 0;
      int hive_hum = 0;
      int hive_bat = 0;
      int hive_weight = 0;
      sscanf(buf, "%8s,%4d,%3d,%3d,%6d", hive_id, &hive_temp, &hive_hum, &hive_bat, &hive_weight );
      SerialMon.print("- temp: ");
      SerialMon.println(hive_temp);
      SerialMon.print("- hum: ");
      SerialMon.println(hive_hum);
      SerialMon.print("- bat: ");
      SerialMon.println(hive_bat);
      SerialMon.print("- id: ");
      SerialMon.println(hive_id);
      SerialMon.print("- weight: ");
      SerialMon.println(hive_weight);
    }
  }
}