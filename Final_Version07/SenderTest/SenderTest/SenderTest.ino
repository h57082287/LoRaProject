#include <LoRa.h>
int packetSize = 11 ;
byte *data = new byte[packetSize];
int maxPkt = 50 ;
int num = 0 ;

void setup() {
 Serial.begin(9600);
  if(!LoRa.begin(915E6))
  {
    Serial.println("Start Fail~");
    while(1);
  }

  // 存放資料
  for(int i = 0 ; i  < packetSize ; i ++)
  {
    data[i] = i ;
  }
}

void loop() {
  if(num < maxPkt)
  {
    long startTime = millis();
    LoRa.beginPacket();
    LoRa.write(data,packetSize);
    LoRa.endPacket();
    long endTime = millis();
    Serial.print(endTime-startTime);
    Serial.print("ms , data : ");
    for(int i = 0 ; i < packetSize ; i++)
    {
      Serial.print("0x");
      Serial.print(data[i],HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
    delay(1000);
    num += 1 ;
  }
}
