#include "MyLoRaForRelay.h"
#include <LoRa.h>
#include <SPI.h>
#include "Arduino.h"
#include <EEPROM.h>

// 定義封包類型
#define PKT_TYPE_DATA 0x01
#define PKT_TYPE_ACK 0x02
#define PKT_TTL_DROP 0x00

// 建立建構子(轉送)
MyLoRaForRelay::MyLoRaForRelay(byte relay, int packetSize , int dtime_min , int dtime_max , int atime_min , int atime_max , int relayCount )
{
  this->setRelay(relay);
  this->packetSize = packetSize ;
  this->setNULL(this->queue,this->packetSize);
  this->setNULL(this->data,this->packetSize);
  this->dtime_min = dtime_min ;
  this->dtime_max = dtime_max ;
  this->atime_min = atime_min ;
  this->atime_max = atime_max ;
  this->relayCount = relayCount ;
}

// 設定Relay位置
void MyLoRaForRelay::setRelay(byte Address = 0x00)
{
  this->relay = Address ;
}

// 發送封包
void MyLoRaForRelay::SendPacket(bool Print)
{
  if(!this->isNULL(this->data,this->packetSize))
  {
    LoRa.beginPacket();
    LoRa.write(this->data,packetSize);
    LoRa.endPacket(); 
    this->Record(this->data);
  }
  if(Print)
  {
    this->PrintPacket(this->data,11,"Send Packet Content :");
  }
  this->setNULL(this->queue,this->packetSize);
  this->setNULL(this->data,this->packetSize);
}

// 接收封包
bool MyLoRaForRelay::getPacket(bool Print)
{
  int backPacketSize = LoRa.parsePacket();
  if(backPacketSize > 0)
  {
    LoRa.readBytes(this->backData , this->packetSize);
    this->Record(this->backData);
    if(Print)
    {
      this->PrintPacket(this->backData,11,"Get Packet Content : ");
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
  if(this->getPacket(false))
  {
    this->process();
  }
  this->EndTime = millis();
  if(!this->isNULL(this->queue,this->packetSize) && (((this->EndTime - this->StartTime) >= this->timeout) || (this->EndTime - this->StartTime) < 0))
  {
    // 將queue的暫存封包寫入正式發送區data
    this->CopyArray(this->queue, this->data, this->packetSize);
//    for(int i = 0 ; i < this->packetSize ; i++)
//    {
//      this->data[i] = this->queue[i] ; 
//    }
    // 設定relay id 及 將ttl檢查碼扣一次
    this->data[2] = this->relay ;
    this->data[5] -= 1 ;
    this->encodeCheckSum();
    this->SendPacket(false);
  }
}

// 處理封包
void MyLoRaForRelay::process()
{
  if((this->backData[5] > PKT_TTL_DROP))
  {
    if(this->isNULL(this->queue,this->packetSize))
    {
//       Serial.println("Mode 1");
       this->CopyArray(this->backData, this->queue, this->packetSize);
//       for(int i = 0 ; i < this->packetSize ; i++)
//       {
//          this->queue[i] = this->backData[i] ; 
//       }
//       this->PrintPacket(this->queue,11,"Queue Data Content : ");
       randomSeed(analogRead(0));
       this->StartTime = millis() ;
       switch(this->queue[3])   
       {
          case PKT_TYPE_DATA :
            this->timeout = random(this->dtime_min,this->dtime_max);
            break;
          case PKT_TYPE_ACK :
            this->timeout = random(this->atime_min,this->atime_max);
            break;
       }
    }
    else if((!this->isNULL(this->queue,this->packetSize)) && (this->queue[3] == PKT_TYPE_DATA))
    {
//       Serial.println("Mode 2");
       if((this->backData[3] == PKT_TYPE_ACK) && (this->backData[4] == this->queue[4]))
       {
//          Serial.println(2);
          this->CopyArray(this->backData, this->queue, this->packetSize);
//          for(int i = 0 ; i < this->packetSize ; i++)
//          {
//             this->queue[i] = this->backData[i] ; 
//          }
          randomSeed(analogRead(0));
          this->StartTime = millis() ;
          this->timeout = random(this->atime_min,this->atime_max);
       }
       else if(this->backData[4] > this->queue[4])
       {
//           Serial.println(3);
           this->CopyArray(this->backData, this->queue, this->packetSize);
//           for(int i = 0 ; i < this->packetSize ; i++)
//           {
//              this->queue[i] = this->backData[i] ; 
//           }
           randomSeed(analogRead(0));
           this->StartTime = millis() ;
           switch(this->backData[3])
           {
            case PKT_TYPE_DATA:
              this->timeout = random(this->dtime_min,this->dtime_max);
              break ;
            case PKT_TYPE_ACK:
              this->timeout = random(this->atime_min,this->atime_max);
              break ;
           }
       }
    }
    else if((!this->isNULL(this->queue,this->packetSize)) && (this->queue[3] == PKT_TYPE_ACK))
    {
//       Serial.println("Mode 3");
       if(this->backData[4] > this->queue[4])
       {
//          Serial.println(4);
          this->CopyArray(this->backData, this->queue, this->packetSize);
//          for(int i = 0 ; i < this->packetSize ; i++)
//          {
//             this->queue[i] = this->backData[i] ; 
//          }
          randomSeed(analogRead(0));
          this->StartTime = millis() ;
          switch(this->backData[3])
           {
            case PKT_TYPE_DATA:
              this->timeout = random(this->dtime_min,this->dtime_max);
              break ;
            case PKT_TYPE_ACK:
              this->timeout = random(this->atime_min,this->atime_max);
              break ;
           }
       }
    }
  }
  //this->PrintPacket(this->queue,this->packetSize,"Queue Content : ");
}

void MyLoRaForRelay::CopyArray(byte Data1[] , byte Data2[] , int Size)
{
  for(int i = 0 ; i < Size ; i++)
  {
    Data2[i] = Data1[i] ;
  }
}

void MyLoRaForRelay::setNULL(byte data[],int len)
{
  for(int i = 0 ; i < len ; i++)
  {
    data[i] = 0 ;
  }
}

bool MyLoRaForRelay::isNULL(byte data[],int len)
{
  for(int i = 0 ; i < len ; i++)
  {
    if(data[i] != 0x00)
    {
      return false;
    }
  }
  return true;
}

// 編碼檢查碼
void MyLoRaForRelay::encodeCheckSum()
{
  uint32_t acc ;
  uint16_t src ;
  uint8_t *octetptr;
  uint8_t checksum_1,checksum_2;
  int len = this->packetSize ;

  // 重置CheckSum
  this->data[6] = 0x00 ;
  this->data[7] = 0x00 ; 
  
  acc = 0;
  octetptr = (uint8_t*)this->data;
  while (len > 1) {
    src = (*octetptr) << 8;
    octetptr++;
    src |= (*octetptr);
    octetptr++;
    acc += src;
    len -= 2;
  }
  if (len > 0) {
    src = (*octetptr) << 8;
    acc += src;
  }

  acc = (acc >> 16) + (acc & 0x0000ffffUL);
  if ((acc & 0xffff0000UL) != 0) {
    acc = (acc >> 16) + (acc & 0x0000ffffUL);
  }

  src = (uint16_t)acc;
  checksum_1 = (~src >> 0) & 0xff ;
  checksum_2 = (~src >> 8) & 0xff ;
  this->data[6] = checksum_2 ;
  this->data[7] = checksum_1 ;
}

// 紀錄
void MyLoRaForRelay::Record(byte d[])
{
  long timestamp = millis();
  this->LogTable[this->Log_idx].timestamp = timestamp ;
  for(int i = 0 ; i < this->packetSize ; i++)
  {
    this->LogTable[this->Log_idx].data[i] = d[i] ;
  }
  if(this->Log_idx >= Log_Max)
  {
    this->ExportRecord();
  }
  this->Log_idx++ ;
}

// 匯出紀錄
void MyLoRaForRelay::ExportRecord()
{
    this->Log_idx = 0 ;
    for(int i = 0 ; i < Log_Max ; i++)
    {
      Serial.print(this->LogTable[i].timestamp);
      Serial.print(" , ");
      for(int j = 0 ; j < this->packetSize ; j++)
      {
        Serial.print(this->LogTable[i].data[j],HEX); 
        Serial.print(' ');
      } 
      Serial.print('\n');
    }
    Serial.println("-------------------------------------------------------------------------------------------------------");

//    // 清空紀錄
//    for(int i = 0 ; i < Log_Max ; i++)
//    {
//      this->LogTable[i].timestamp = 0;
//      for(int j = 0 ; j < this->packetSize ; j++)
//      {
//        this->LogTable[i].data[j] = 0x00; 
//      } 
//    }
}
