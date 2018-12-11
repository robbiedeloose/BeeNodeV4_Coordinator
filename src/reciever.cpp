#include "reciever.h"

void getDataFromReciever(){
  SerialMon.println(":: getDataFromReciever");
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //SerialMon.println("low");
  digitalWrite(WAKE_RECIEVER, LOW);
  delay(100);
  //SerialMon.println("high");
  digitalWrite(WAKE_RECIEVER, HIGH);
  delay(5000);

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
    }
    /*char id[9];
    int temp = 0;
    int hum = 0;
    int bat = 0;
    int weight = 0;
    sscanf(buf, "%8s%4d%3d%3d%6d", id, &temp, &hum, &bat, &weight );
    Serial.print("temp: ");
    Serial.println(temp);
    Serial.print("hum: ");
    Serial.println(hum);
    Serial.print("bat: ");
    Serial.println(bat);
    Serial.print("id: ");
    Serial.println(id);
    */
  }
}