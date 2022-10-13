#include "MyLoRaForRelay.h"
#include <LoRa.h>
#include <SPI.h>
#include "Arduino.h"
#include <EEPROM.h>

/*
 * 1. 使用EEPROM 儲存 SQ Number 避免關機重開後無法正常傳值
 * 2. 使用Data Table 確保資料不會接收到相同內容
 */
// 建立建構子(轉送)
MyLoRaForRelay::MyLoRaForRelay(byte relay, int packetSize)
{
  this->setRelay(relay);
  this->packetSize = packetSize ;
}

// 設定Relay位置
void MyLoRaForRelay::setRelay(byte Address = 0x00)
{
  this->relay = Address ;
}

// 發送封包
void MyLoRaForRelay::SendPacket(bool Print)
{
  LoRa.beginPacket();
  LoRa.write(this->data,packetSize);
  LoRa.endPacket();
  if(Print)
  {
    this->PrintPacket(this->data,11,"Send Packet Content :");
    //this->SaveLogs("Send.txt",this->data);
  }
}


// 接收封包
bool MyLoRaForRelay::getPacket(bool Print)
{
  int backPacketSize = LoRa.parsePacket();
  if(backPacketSize > 0)
  {
    LoRa.readBytes(this->backData , backPacketSize);
    if(Print)
    {
      this->PrintPacket(this->backData,11,"Get Packet Content : ");
      //SaveLogs("ACKLogs.txt",this->backData);
    }
    return true; 
  }
  return false;
}

// 建立顯示封包
void MyLoRaForRelay::PrintPacket(byte d[],int Max , String msg)
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


// 啟動轉送器
void MyLoRaForRelay::RelayBegin()
{
  if(this->getPacket(true))
  {
    for(int i = 0 ; i < 11 ; i++)
    {
      this->data[i] = this->backData[i]; 
    }
    this->data[2] = this->relay;
    delay(2000);
    if(this->data[5] != 0x00)
    {
      this->data[5] -= 1 ;
      this->SendPacket(true); 
    }
  }
}
