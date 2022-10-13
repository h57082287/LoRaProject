#ifndef _MyLoRa__
#define _MyLoRa__

#include <LoRa.h>
#include <SPI.h>
#include "Arduino.h"

// 建構LoRa物件
class MyLoRa
{
  private :
    byte sender , revicer , relay , num = 0 ;
    int packetSize ,limit = 3;
    long SendTime , GetTime ,SendACKTime,GetACKTime;
    byte historyData[255][3] = {0};
    byte index = 0 ;
    byte ResendNum = 0 ;
    
    // 產生亂數資料
    void RandomData(int MaxNum);
    
  public :
    byte data[11] ;
    byte backData[11];
    byte d[6] ;
    
    // 建立建構子(節點)
    MyLoRa(byte sender , byte revicer , int packetSize);

    // 建立建構子(轉送)
    MyLoRa(byte relay, int packetSize);

    // 內部計數器 + 1 
    void setNext();

    // 設定標頭資料
    void setHeader(byte S_A);

    // 設定封包資料
    void setData(byte mode , int MaxNum = 6 ,byte inputData[] = {0});

    // 設定Relay位置
    void setRelay(byte Address = 0x00);

    // 發送封包
    void SendPacket(bool Print);
    
    // 接收封包
    bool getPacket(bool Print);

    // 清除封包
    void claerBuffer();

    // 列印封包
    void PrintPacket(byte d[] ,int Max , String msg);

    // 發送回應封包
    void SendACK(bool Print);

    // 接收回應封包
    bool GetACK(bool Print);

    // 建立轉送器
    void RelayBegin();

    // 設定整個封包內容
    void setPacket(byte input[]);

    // 搜尋歷史資料
    bool SearchHistory(byte nowPacket[]);

    // 列印出紀錄
    void PrintSeacherHistory();

    // 記錄至 EEPROM
    void SetToEEPROM(int number);

    // 從EEPROM提取紀錄
    int GetFromEEPROM();
    
};

#endif
