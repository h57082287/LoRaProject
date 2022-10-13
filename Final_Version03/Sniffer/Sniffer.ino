#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  if(!LoRa.begin(915E6))
  {
    Serial.println("Fail");
    while(1);
  }
  Serial.println("Sniffer Address : 0x00");

}

void loop() {
  byte backData[11];
  int backPacketSize = LoRa.parsePacket();
  if(backPacketSize > 0)
  {
    Serial.print("Get Packet Content : ");
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
