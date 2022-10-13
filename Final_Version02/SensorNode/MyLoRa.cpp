#include "MyLoRa.h"
#include <LoRa.h>
#include <SPI.h>
#include "Arduino.h"
#include <EEPROM.h>

/*
 * 1. 使用EEPROM 儲存 SQ Number 避免關機重開後無法正常傳值
 * 2. 使用Data Table 確保資料不會接收到相同內容
 */
// 建立亂數功能
void MyLoRa::RandomData(int MaxNum)
{
  for(int i = 0 ; i < MaxNum ; i++)
  {
    randomSeed(analogRead(0));
    this->d[i] =random(97,122);
  }
}

// 建立建構子(節點)
MyLoRa::MyLoRa(byte sender , byte revicer , int packetSize)
{
  this->sender = sender;
  this->revicer = revicer;
  this->packetSize = packetSize ;
  this->num = this->GetFromEEPROM();
}

// 建立建構子(轉送)
MyLoRa::MyLoRa(byte relay, int packetSize)
{
  this->setRelay(relay);
  this->packetSize = packetSize ;
}

// 建立計數器
void MyLoRa::setNext()
{
  num ++ ;
  num = num % 255 ;
  this->SetToEEPROM(num);
}

// 設定標頭資料
void MyLoRa::setHeader(byte S_A)
{
  if(S_A == 0x02)
  {
    this->data[0] = this->sender;
    this->data[1] = this->backData[0];
    this->data[2] = this->relay;
    this->data[3] = S_A ;
    this->data[4] = this->data[4] ;
  }
  else
  {
    this->data[0] = this->sender;
    this->data[1] = this->revicer;
    this->data[2] = this->relay;
    this->data[3] = S_A ;
    this->data[4] = this->num ;
  }
}

// 設定封包資料
void MyLoRa::setData(byte mode , int MaxNum = 6 ,byte inputData[] = {0})
{
  if (mode == 1)
  {
    this->RandomData(MaxNum);
    for (int i = 0 ; i < MaxNum ; i++ )
    {
      this->data[i+5] = this->d[i];
    }
  }
  if (mode == 2)
  {
    for(int i = 5 ; i <= 11 ; i++)
    {
      this->data[i] = inputData[i-5];
    }
  }
  if(mode == 3)
  {
    for(int i = 4 ; i <=11 ; i++)
    {
      this->data[i] = this->backData[i];
    }
  }
}

// 設定Relay位置
void MyLoRa::setRelay(byte Address = 0x00)
{
  this->relay = Address ;
}

// 發送封包
void MyLoRa::SendPacket(bool Print)
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
bool MyLoRa::getPacket(bool Print)
{
  int backPacketSize = LoRa.parsePacket();
  if(backPacketSize > 0)
  {
    LoRa.readBytes(this->backData , backPacketSize);
    if(this->backData[1] == this->sender || this->relay != 0x00)
    {
      byte nowPacket[3] = {this->backData[0] , this->backData[1] , this->backData[4]};
      if(!SearchHistory(nowPacket))
      {
        // 資料放入紀錄中
        this-> index = this-> index % 255;
        this-> historyData[this->index][0] = this->backData[0];
        this-> historyData[this->index][1] = this->backData[1];
        this-> historyData[this->index][2] = this->backData[4];
        this-> index += 1;
        
        if(Print)
        {
          this->PrintPacket(this->backData,11,"Get Packet Content : ");
          //SaveLogs("ACKLogs.txt",this->backData);
        }
        return true; 
      }
    }
  }
  return false;
}

// 清除封包
void MyLoRa::claerBuffer()
{
  this->ResendNum = 0 ;
  for(int i = 0 ; i < 11 ; i++)
  {
    data[i] = 0x00 ;
    backData[i] = 0x00 ;
    d[i] = 0x00 ;
  }
}

// 建立顯示封包
void MyLoRa::PrintPacket(byte d[],int Max , String msg)
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

// 發送回應封包
void MyLoRa::SendACK(bool Print)
{
  delay(1000);
  this->SendACKTime = millis();
  this->setHeader(0x02);
  this->setData(0x03);
  this->SendPacket(Print);
}

// 接收回應封包
bool MyLoRa::GetACK(bool Print)
{
  this->GetACKTime = millis();
  this->getPacket(Print);
  if(((this->backData[1] == this->sender) && (this->backData[4] == this->num) && (this->backData[3] == 0x02)) || (this->ResendNum > this->limit ))
  {
    this->claerBuffer();
    return true;
  }
  if((this->GetACKTime - this->SendTime) > 2000)
  {
    this->SendPacket(true);
    this->ResendNum += 1 ;
  }
  return false;
}

// 啟動轉送器
void MyLoRa::RelayBegin()
{
  if(getPacket(true))
  {
    for(int i = 0 ; i < 11 ; i++)
    {
      this->data[i] = this->backData[i]; 
    }
    this->data[2] = this->relay;
    this->SendPacket(true);
  }
}

void MyLoRa::setPacket(byte input[])
{
  // 設定Header
  for(int i = 0 ; i < 5 ; i++)
  {
    this->data[i] = input[i];
  }

  byte _input[6];
  for(int i = 5 ; i < 11 ; i++)
  {
    _input[i] = input[i];
  }
  
  // 設定資料
  this->setData(0x02,6,_input);
  
}

// 搜尋歷史紀錄
bool MyLoRa::SearchHistory(byte nowPacket[])
{
  //Serial.println("This is [SearchHistory] Function");
  for(int i = 0 ; i < 255 ; i++)
  {
    int replyNum = 0 ;
    for(int j = 0 ; j < 3 ; j++)
    {
      if(this ->historyData[i][j] == nowPacket[j])
      {
        replyNum += 1 ;
      }
    }
    if(replyNum >= 3)
    {
      return true;
    }
  }
  return false;
}


// 列印出紀錄
void MyLoRa::PrintSeacherHistory()
{
  for(int i = 0 ; i < 255 ; i++)
  {
    Serial.print("[");
    for(int j = 0 ; j < 3 ; j++)
    {
      Serial.print("0x");
      Serial.print(this->historyData[i][j],HEX);
      Serial.print(" ");
    }
    Serial.println("]");
  }
}


// 記錄至 EEPROM
void MyLoRa::SetToEEPROM(int number)
{
  EEPROM.write(0,number);
}


// 從EEPROM提取紀錄
int MyLoRa::GetFromEEPROM()
{
  return EEPROM.read(0);
}
