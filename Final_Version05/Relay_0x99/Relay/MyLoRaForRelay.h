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
    int relayCount = 0 ;
    
  public :
    byte data[11];
    byte backData[11];
    byte queue[11];
    typedef struct{
      byte control ;
      byte seqNum ;
      byte ttl ; 
    } PktInfo ;

    // 建立建構子(轉送)
    MyLoRaForRelay(byte relay, int packetSize , int dtime_min , int dtime_max , int atime_min , int atime_max , int relayCount);

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

    // 複製陣列資料
    void CopyArray(byte Data1[] , byte Data2[] , int Size);

    // 設為空值
    void setNULL(byte data[],int len);

    // 檢查是否空值
    bool isNULL(byte data[],int len);
};

#endif
