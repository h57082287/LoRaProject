// LoRa 發送端

// LoRa模組引用
#include<LoRa.h>
#include<SPI.h>
// 設定全域變數
String msg = "okok";

// 初始化設定
void setup() {
  Serial.begin(9600);
  if(!LoRa.begin(915E6))
  {
    Serial.println("Start Fail~");
    while(1);
  }
  Serial.println("LoRa Start");
}

void loop() {
   String a = Serial.readString();
   if(a != "")
    {
      Serial.print("送出資料為:" + a);
      SendPacket(a);
    }
}


void SendPacket(String msg)
{
  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();
}
