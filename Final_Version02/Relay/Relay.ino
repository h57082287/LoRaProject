#include "MyLoRa.h"
#include <SPI.h>
#include "Arduino.h"
#include <LoRa.h>

// 定義MyLoRa物件
MyLoRa L(0x98,11);

// 定義全域變數
bool lock = false ;

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

void AutoSend()
{
  L.setHeader(0x01);
  L.setData(0x01);
  L.SendPacket(true);
  while(!L.GetACK(true));
  L.setNext();
}

// 通用設定區
// ===================================================================
void setup()
{
  Serial.begin(9600);
  if(!LoRa.begin(915E6))
  {
    Serial.println("Start Fail~");
    while(1);
  }
  Serial.println("I'm Relay");
  Serial.setTimeout(500);
}

// 通用迴圈區
// ===================================================================
void loop()
{
   L.RelayBegin(); 
}
