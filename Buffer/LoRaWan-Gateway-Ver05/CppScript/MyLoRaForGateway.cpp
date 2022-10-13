#include "MyLoRaForGateway.h"
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

// 建立建構子(節點)
MyLoRaForGateway::MyLoRaForGateway(unsigned char sender, int packetSize , int relayNum)
{
  this->sender = sender;
  this->packetSize = packetSize ;
  this->relayNum = relayNum ;
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

// 設定標頭資料
void MyLoRaForGateway::setHeader()
{
    this->data[0] = this->sender;
    this->data[1] = this->backData[0];
    this->data[2] = 0x00;
    this->data[3] = 0x02 ;
    this->data[4] = this->backData[4] ;
    this->data[5] = this->relayNum;
}

// 設定封包資料
void MyLoRaForGateway::setData()
{
    for(int i = 6 ; i < this->packetSize ; i++)
    {
      this->data[i] = this->backData[i];
    }
    // sensorNode 不檢查ACK封包的CheckSum
    // data包含checksum都不會變更
}


// 發送封包
void MyLoRaForGateway::SendPacket(bool Print)
{
  lora.transmitPacket(this->data,packetSize);
  if(Print)
  {
    this->PrintPacket(data,this->packetSize,"Send Packet Content : ");
  }
}


// 接收封包
bool MyLoRaForGateway::getPacket(bool Print)
{
  size_t backPacketSize = lora.receivePacket(this->backData);
  if(backPacketSize > 0)
  {
    if((this->backData[1] == this->sender) && (backData[3] == 0x01) && (this->decodeCheckSum()))
    {
      this->SendACK(false); // 添加時間2022-7-20--21-16
      if(Print)
      {
        this->PrintPacket(this->backData,this->packetSize,"Get  Packet Content : ");
      }
      return true; 
    }
  }
  return false;
}

// 建立顯示封包
void MyLoRaForGateway::PrintPacket(unsigned char d[],int Max , std::string msg)
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
void MyLoRaForGateway::SendACK(bool Print)
{
  unsigned char k[6] = {0,0,0,0,0,0};
  //usleep(1000);
  this->setHeader();
  this->setData();
  this->SendPacket(Print);
}

// 解碼檢查碼
bool MyLoRaForGateway::decodeCheckSum()
{
  uint32_t acc;
  uint16_t src;
  uint8_t *octetptr;
  uint8_t checksum_1 , checksum_2;
  int len = this->packetSize - 2 ;

  acc = 0;
  octetptr = (uint8_t*)this->backData;
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
  if((checksum_1 == this->backData[this->packetSize-1]) && (checksum_2 == this->backData[this->packetSize-2]) )
  {
    return true;
  }
  return false ;
}

