// LoRa 接收端

// LoRa模組引用
#include<LoRa.h>
#include<SPI.h>

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
  int PacketSize = LoRa.parsePacket();
  if(PacketSize > 0)
  {
    while(LoRa.available())
    {
      byte packet[11];
      LoRa.readBytes(packet,11);
      PrintPacket(packet,11,"Packet :");
    }
  }
}


void PrintPacket(byte d[],int Max,String msg)
{
  Serial.print(msg);
  for(int i = 0 ; i < Max ; i++)
  {
    Serial.print("0x");
    Serial.print(d[i],HEX);
    Serial.print(" ");
  }
  Serial.println("");
}
