#include "MyLoRaForSensorNode.h"
#include <LoRa.h>
#include <SPI.h>
#include "Arduino.h"
#include <EEPROM.h>

/*
 * 1. 使用EEPROM 儲存 SQ Number 避免關機重開後無法正常傳值
 * 2. 使用Data Table 確保資料不會接收到相同內容
 */
// 建立亂數功能
void MyLoRaForSensorNode::RandomData(int MaxNum)
{
  for(int i = 0 ; i < MaxNum ; i++)
  {
    randomSeed(analogRead(0));
    this->d[i] =random(97,122);
  }
}

// 建立建構子(節點)
MyLoRaForSensorNode::MyLoRaForSensorNode(byte sender , byte revicer , int packetSize , int relayNum)
{
  this->sender = sender;
  this->revicer = revicer;
  this->packetSize = packetSize ;
  this->relayNum = relayNum ;
  this->num = this->GetFromEEPROM();
}

// 建立計數器
void MyLoRaForSensorNode::setNext()
{
  num ++ ;
  num = num % 255 ;
  this->SetToEEPROM(num);
}

// 設定標頭資料
void MyLoRaForSensorNode::setHeader()
{
    this->data[0] = this->sender;
    this->data[1] = this->revicer;
    this->data[2] = 0x00;
    this->data[3] = 0x01 ;
    this->data[4] = this->num ;
    this->data[5] = this->relayNum ;
}

// 設定封包資料
void MyLoRaForSensorNode::setData(byte mode , int MaxNum = 5 ,byte inputData[] = {0})
{
  if (mode == 1)
  {
    this->RandomData(MaxNum);
    for (int i = 0 ; i < MaxNum ; i++ )
    {
      this->data[i+6] = this->d[i];
    }
  }
  if (mode == 2)
  {
    for(int i = 6 ; i <= 11 ; i++)
    {
      this->data[i] = inputData[i-5];
    }
  }
}

// 發送封包
void MyLoRaForSensorNode::SendPacket(bool Print)
{
  LoRa.beginPacket();
  LoRa.write(this->data,packetSize);
  LoRa.endPacket();
  this->SendTime = millis();
  if(Print)
  {
    this->PrintPacket(this->data,11,"Send Packet Content :");
    //this->SaveLogs("Send.txt",this->data);
  }
}


// 接收封包
bool MyLoRaForSensorNode::getPacket(bool Print)
{
  int backPacketSize = LoRa.parsePacket();
  if(backPacketSize > 0)
  {
      LoRa.readBytes(this->backData , this->packetSize);
      return true; 
   }
  return false;
}

// 清除封包
void MyLoRaForSensorNode::claerBuffer()
{
  this->ResendNum = 0 ;
  for(int i = 0 ; i < this->packetSize ; i++)
  {
    data[i] = 0x00 ;
    backData[i] = 0x00 ;
    d[i] = 0x00 ;
  }
}

// 建立顯示封包
void MyLoRaForSensorNode::PrintPacket(byte d[],int Max , String msg)
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

// 接收回應封包
bool MyLoRaForSensorNode::GetACK(bool Print)
{
  this->GetACKTime = millis();
  if(this->getPacket(false))
  {
    if(this->ResendNum > this->limit )
    {
      this->claerBuffer();
      return true;
    }
    else if((this->backData[1] == this->sender) && (this->backData[4] == this->num) && (this->backData[3] == 0x02))
     {
        if(Print)
        {
          this->PrintPacket(this->backData,11,"Get Packet Content : ");
        }
        this->claerBuffer();
        return true;
      }
  }
  if((this->GetACKTime - this->SendTime) > 10000)
    {
      this->ResendNum += 1 ;
      this->SendPacket(true);
    } 
  return false;
}

// 記錄至 EEPROM
void MyLoRaForSensorNode::SetToEEPROM(int number)
{
  EEPROM.write(0,number);
}


// 從EEPROM提取紀錄
int MyLoRaForSensorNode::GetFromEEPROM()
{
  return EEPROM.read(0);
}
