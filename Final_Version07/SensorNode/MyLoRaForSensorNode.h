#ifndef _MyLoRa__
#define _MyLoRa__

#include <LoRa.h>
#include <SPI.h>
#include "Arduino.h"

#define Log_Max 50
#define PayloadSize 11

// 建構LoRa物件
class MyLoRaForSensorNode
{
  private :
    byte sender , revicer , relayNum , num = 0 ;
    int packetSize ,limit = 3;
    long SendTime ,GetACKTime;
    byte ResendNum = 0 ;
    int Log_idx = 0 ;
    typedef struct{
      long timestamp ;
      byte data[PayloadSize];
    }Log;
    Log LogTable[Log_Max];
    
    // 產生亂數資料
    void RandomData(int MaxNum);
    
  public :
    byte data[11] ;
    byte backData[11];
    byte d[6] ;
    
    // 建立建構子(節點)
    MyLoRaForSensorNode(byte sender , byte revicer , int packetSize , int relayNum);

    // 內部計數器 + 1 
    void setNext();

    // 設定標頭資料
    void setHeader();

    // 設定封包資料
    void setData(byte mode , int MaxNum = 3 ,byte inputData[] = {0});

    // 發送封包
    void SendPacket(bool Print);
    
    // 接收封包
    bool getPacket(bool Print);

    // 清除封包
    void claerBuffer();

    // 列印封包
    void PrintPacket(byte d[] ,int Max , String msg);

    // 接收回應封包
    bool GetACK(bool Print);

    // 記錄至 EEPROM
    void SetToEEPROM(int number);

    // 從EEPROM提取紀錄
    int GetFromEEPROM();

    // 編碼檢查碼
    void encodeCheckSum();

    // 紀錄
    void Record(byte d[]);
};

#endif
