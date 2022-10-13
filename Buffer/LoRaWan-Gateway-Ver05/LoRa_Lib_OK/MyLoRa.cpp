#include "MyLoRa.h"
#include "lora.h"
#include <string>
#include <ctime>
#include <cstdio>
#include <memory>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>



#define SPI_CHANNEL 0
#define SS_PIN      6
#define DIO0_PIN    7
#define RST_PIN     0

LoRa lora(SPI_CHANNEL, SS_PIN, DIO0_PIN, RST_PIN);

// 建立亂數功能
// void MyLoRa::RandomData(int MaxNum)
// {
  
//   for(int i = 0 ; i < MaxNum ; i++)
//   {
//     randomSeed(analogRead(0));
//     this->d[i] =random(97,122);
//   }
// }

// 建立建構子(節點)
MyLoRa::MyLoRa(unsigned char sender , unsigned char revicer , int packetSize)
{
  this->sender = sender;
  this->revicer = revicer;
  this->packetSize = packetSize ;
  printf("Setting up LoRa\n");
	if (lora.begin()) 
  {
		printf("LoRa setup successful: chipset version 0x%02x\n", lora.version());
		printf("Configuring radio\n");
		lora.setFrequency(LoRa::FREQ_915)
			->setTXPower(17)
			->setSpreadFactor(LoRa::SF_7)
			->setBandwidth(LoRa::BW_125k);
		printf("  TX power     : %d dB\n", lora.getTXPower());
		printf("  Frequency    : %d Hz\n", lora.getFrequency());
		printf("  Spread factor: %d\n", lora.getSpreadFactor());
		printf("  Bandwidth    : %d Hz\n", lora.bw[lora.getBandwidth()]);
		printf("  Coding Rate  : 4/%d\n", lora.getCodingRate() + 4);
		printf("  Sync word    : 0x%02x\n", lora.getSyncWord());
		printf("  Header mode  : %s\n", lora.getHeaderMode() == LoRa::HM_IMPLICIT ? "Implicit" : "Explicit");
  }
  printf("Inin OK\n");
}

// 建立建構子(轉送)
MyLoRa::MyLoRa(unsigned char relay, int packetSize)
{
  this->setRelay(relay);
  this->packetSize = packetSize ;
}

// 建立計數器
void MyLoRa::setNext()
{
  num ++ ;
  num = num % 255 ;
}

// 設定標頭資料
void MyLoRa::setHeader(unsigned char S_A)
{
  if(S_A == 0x02)
  {
    this->data[0] = this->sender;
    this->data[1] = this->backData[0];
    this->data[2] = this->relay;
    this->data[3] = S_A ;
    this->data[4] = this->backData[4] ;
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
void MyLoRa::setData(unsigned char mode , int MaxNum ,unsigned char *inputData)
{
  if (mode == 1)
  {
    //this->RandomData(MaxNum);
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
    for(int i = 5 ; i < 11 ; i++)
    {
      this->data[i] = this->backData[i];
    }
  }
}

// 設定Relay位置
void MyLoRa::setRelay(unsigned char Address)
{
  this->relay = Address ;
}


// 發送封包
void MyLoRa::SendPacket(bool Print)
{
  lora.transmitPacket(this->data,packetSize);
  //LoRa.write(data,packetSize);
  this->SendTime = time(NULL);
  if(Print)
  {
    this->PrintPacket(data,11,"Send Packet Content : ");
  }
}


// 接收封包
bool MyLoRa::getPacket(bool Print)
{
  size_t backPacketSize = lora.receivePacket(this->backData);
  if(backPacketSize > 0)
  {
    if(this->backData[1] == this->sender || this->relay != 0x00)
    {
     this->SendACK(true); // 添加時間2022-6-15--13-41
      unsigned char nowPacket[3] = {this->backData[0] , this->backData[1] , this->backData[4]};
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
          this->PrintPacket(this->backData,this->packetSize,"Get  Packet Content : ");
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
  for(int i = 0 ; i < 11 ; i++)
  {
    data[i] = 0x00 ;
    backData[i] = 0x00 ;
    d[i] = 0x00 ;
  }
}

// 建立顯示封包
void MyLoRa::PrintPacket(unsigned char d[],int Max , std::string msg)
{
  printf("%s",msg.c_str());
  for(int i = 0 ; i < Max ; i++)
  {
    printf("0x");
    printf("%x",d[i]);
    printf(" ");
  }
  printf("\n");
}

// 發送回應封包
void MyLoRa::SendACK(bool Print)
{
  unsigned char k[6] = {0,0,0,0,0,0};
  usleep(1000);
  this->SendACKTime = time(NULL);
  this->setHeader(0x02);
  this->setData(0x03,6,k);
  this->SendPacket(Print);
}

// 接收回應封包
bool MyLoRa::GetACK(bool Print)
{
  this->GetACKTime = time(NULL);
  this->getPacket(Print);
  if((this->backData[1] == this->sender) && (this->backData[4] == this->num) && (this->backData[3] == 0x02))
  {
    this->claerBuffer();
    return true;
  }
  if((this->GetACKTime - this->SendTime) > 2000)
  {
    this->SendPacket(true);
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

void MyLoRa::setPacket(unsigned char input[])
{
  // 設定Header
  for(int i = 0 ; i < 5 ; i++)
  {
    this->data[i] = input[i];
  }

  unsigned char _input[6];
  for(int i = 5 ; i < 11 ; i++)
  {
    _input[i] = input[i];
  }
  
  // 設定資料
  this->setData(0x02,6,_input);
  
}

// 搜尋歷史紀錄
bool MyLoRa::SearchHistory(unsigned char nowPacket[])
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
    printf("[");
    for(int j = 0 ; j < 3 ; j++)
    {
      printf("0x");
      printf(" %x ",this->historyData[i][j]);
    }
    printf("]\n");
  }
}
