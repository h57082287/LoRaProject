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
    
  public :
    byte data[11] ;
    byte backData[11];
    byte d[6] ;

    // 建立建構子(轉送)
    MyLoRaForRelay(byte relay, int packetSize);

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
};

#endif
