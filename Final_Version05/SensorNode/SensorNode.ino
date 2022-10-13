#include "MyLoRaForSensorNode.h"
#include <SPI.h>
#include "Arduino.h"
#include <LoRa.h>

// 定義MyLoRa物件
MyLoRaForSensorNode SensorNode(0x01 , 0x02 , 11 , 2);

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
  SensorNode.setHeader();
  SensorNode.setData(0x01);
  SensorNode.encodeCheckSum();
  SensorNode.SendPacket(true);
  while(!SensorNode.GetACK(true));
  SensorNode.setNext();
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
  Serial.println("LoRa Address : 0x01");
  Serial.println("Gateway Address : 0x02");
  Serial.setTimeout(500);
}

// 通用迴圈區
long STime = 0;
long ETime = 3001;
// ===================================================================
void loop()
{
//  if(Serial.available())
//  {
//    byte inputData[6] = {0};
//    Serial.readBytes(inputData,6);
//    L.setHeader(0x01);
//    L.setData(0x02,6,inputData);
//    L.SendPacket(true);
//    while(!L.GetACK(true));
//    L.setNext();
//    //L.PrintSeacherHistory();
//  }
  if(ETime-STime > 5000)
  {
    STime = millis();
    AutoSend();
  }
  ETime = millis();
}
