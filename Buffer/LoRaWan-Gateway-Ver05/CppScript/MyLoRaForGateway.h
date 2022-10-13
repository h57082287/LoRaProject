#ifndef _MyLoRa__
#define _MyLoRa__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <memory>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>


// 建構LoRa物件
class MyLoRaForGateway
{
  private :
    unsigned char sender ;
    int packetSize ,relayNum;
    
  public :
    unsigned char data[11] ;
    unsigned char backData[11];
    unsigned char d[6] ;
    
    // 建立建構子(節點)
    MyLoRaForGateway(unsigned char sender , int packetSize ,int relayNum);

    // 設定標頭資料
    void setHeader();

    // 設定封包資料
    void setData();

    // 發送封包
    void SendPacket(bool Print);
    
    // 接收封包
    bool getPacket(bool Print);

    // 列印封包
    void PrintPacket(unsigned char d[] ,int Max , std::string msg);

    // 發送回應封包
    void SendACK(bool Print);
    
    // 解碼檢查碼
    bool decodeCheckSum();
};

#endif
