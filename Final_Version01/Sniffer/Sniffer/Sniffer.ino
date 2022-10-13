#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  if(!LoRa.begin(915E6))
  {
    while(1);
  }
  Serial.println("Init OK ~");

}

void loop() {
  byte backData[11];
  int backPacketSize = LoRa.parsePacket();
  if(backPacketSize > 0)
  {
    LoRa.readBytes(backData , 11);
    for (int i = 0 ; i < 11 ; i++)
    {
      Serial.print("0x");
      Serial.print(backData[i],HEX);
      Serial.print(" ");
    }
    Serial.println("");
  }

}
