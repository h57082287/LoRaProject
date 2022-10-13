#ifndef _MyLoRa__
#define _MyLoRa__

#include <LoRa.h>
#include <SPI.h>
#include "Arduino.h"

// 建構LoRa物件
class MyLoRaForRelay
{
  private :
    byte relay ;
    int packetSize;
    int timeout = 0 ;
    int dtime_min,dtime_max,atime_min,atime_max ;
    long StartTime , EndTime ;
    
  public :
    byte *data = new byte[this->packetSize] ;
    byte backData[11];
    byte *queue = new byte[this->packetSize];
    typedef struct{
      byte control ;
      byte seqNum ;
      byte ttl ; 
    } PktInfo ;

    // 建立建構子(轉送)
    MyLoRaForRelay(byte relay, int packetSize , int dtime_min , int dtime_max , int atime_min , int atime_max);

    // 設定Relay位置
    void setRelay(byte Address = 0x00);

    // 發送封包
    void SendPacket(bool Print);
    
    // 接收封包
    bool getPacket(bool Print);

    // 列印封包
    void PrintPacket(byte d[] ,int Max , String msg);

    // 建立轉送器
    void RelayBegin();

    // 處理封包
    void process();
};

#endif
