// 引入函數庫
#include<LoRa.h>
#include<SPI.h>

// 建立發送封包
byte packet[12] = {0x01,0x02,0x00,0x01,0x01,0x61,0x62,0x63,0x31,0x32,0x33,0x23};
// 封包格式         From, To,Relay,S/A,  Seq ,--------------data---------------

// 初始化設定
void setup() {
  Serial.begin(9600);
  if(!LoRa.begin(915E6))
  {
    Serial.println("Start Fail~");
    while(1);
  }
  Serial.println("LoRa Start");
  for(int i = 0 ; i < 12 ; i++)
    Serial.write(packet[i]);
}

void loop() {
  PacketOut(packet);
  delay(3000);
}


void PacketOut(byte* data)
{
 
  LoRa.beginPacket();
  LoRa.write(data,12);
  LoRa.endPacket();
  //Serial.println(data,HEX);
}

// 產生亂數資料
byte* RandomData(byte* data)
{
  byte* d = data ;
  int data_len = sizeof(data); 
  // 資料範圍位置5~位置10
  for(int i = 5 ; i <= 10 ; i++)
  {
    data[i] = random(97,122);
  }
  return d;
}
