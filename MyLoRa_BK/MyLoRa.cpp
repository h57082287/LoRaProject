#include "MyLoRa.h"
#include <LoRa.h>
#include <SPI.h>
#include "Arduino.h"

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
}

// 建立建構子(轉送)
MyLoRa::MyLoRa(byte relay)
{
  this->setRelay(relay);
}

// 建立計數器
void MyLoRa::setNext()
{
  num ++ ;
  num = num % 255 ;
}

// 設定標頭資料
void MyLoRa::setHeader(byte S_A)
{
  if(S_A == 0x02)
  {
    this->data[0] = this->sender;
    this->data[1] = this->revicer;
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
    for(int i = 5 ; i <= this->packetSize ; i++)
    {
      this->data[i] = inputData[i-5];
    }
  }
  if(mode == 3)
  {
    for(int i = 4 ; i <=this->packetSize ; i++)
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
  LoRa.write(data,packetSize);
  LoRa.endPacket();
  this->SendTime = millis();
  if(Print)
  {
    this->PrintPacket(data,this->packetSize,"Send Packet Content : ");
  }
}


// 接收封包
bool MyLoRa::getPacket(bool Print)
{
  int backPacketSize = LoRa.parsePacket();
  if(backPacketSize > 0)
  {
    LoRa.readBytes(this->backData , backPacketSize);
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
        this->PrintPacket(backData,this->packetSize,"Get  Packet Content : ");
      }
      return true; 
    }
  }
  return false;
}

// 清除封包
void MyLoRa::claerBuffer()
{
  for(int i = 0 ; i < this->packetSize ; i++)
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
  if((this->backData[1] == this->sender) && (this->backData[4] == this->num) && (this->backData[3] == 0x02))
  {
    this->ResendNum = 0 ;
    this->claerBuffer();
    return true;
  }
  if((this->GetACKTime - this->SendTime) > 2000)
  {
    this->SendPacket(true);
    this->ResendNum += 1 ;
  }
  if(this->ResendNum > 20)
  {
    this->claerBuffer();
    this->ResendNum = 0;
    this->PrintPacket(this->data,this->packetSize,"Error Packet Content : ");
    return true;
  }
  return false;
}

// 啟動轉送器
void MyLoRa::RelayBegin()
{
  if(getPacket(true))
  {
    for(int i = 0 ; i < this->packetSize ; i++)
    {
      this->data[i] = this->backData[i]; 
    }
    this->data[3] = this->relay;
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
  for(int i = 5 ; i < this->packetSize ; i++)
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
