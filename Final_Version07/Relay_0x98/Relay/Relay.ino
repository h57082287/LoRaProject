#include "MyLoRaForRelay.h"
#include <SPI.h>
#include "Arduino.h"
#include <LoRa.h>

// 定義MyLoRa物件
MyLoRaForRelay Relay(0x98,11,100,100,100,100,2);

// 建立函數區
// ===================================================================
// 建立顯示封包
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

// 通用設定區
// ===================================================================
void setup()
{
  Serial.begin(9600);
  pinMode(A3,INPUT);
  if(!LoRa.begin(915E6))
  {
    Serial.println("Start Fail~");
    while(1);
  }
  Serial.println("Relay Address : 0x98");
  Serial.setTimeout(500);
}

// 通用迴圈區
// ===================================================================
void loop()
{
   Relay.RelayBegin(); 
   if(digitalRead(A3) == HIGH)
   {
      Relay.ExportRecord();
   }
}
