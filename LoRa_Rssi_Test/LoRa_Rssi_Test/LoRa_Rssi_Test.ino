#include<LoRa.h>

// 建立LoRa 物件
LoRaClass L1;

// 設定初始化
void setup() {
 Serial.begin(9600);
 L1.begin(915E6);
}


// 持續工作
void loop() {

 int Size = L1.parsePacket();

 if(Size)
 {
  while(L1.available())
  {
    String data = L1.readString();
    Serial.println(data);
    Serial.println(L1.rssi());
    Serial.println("---------------------------");
    delay(500);
    if(data == "okok")
    {
      SendPacket("ok");
    }
  }
 }

}


void SendPacket(String msg)
{
  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();
}
