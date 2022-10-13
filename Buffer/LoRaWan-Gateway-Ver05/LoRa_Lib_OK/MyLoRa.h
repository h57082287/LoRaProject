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
class MyLoRa
{
  private :
    unsigned char sender , revicer , relay , num = 0 ;
    int packetSize ;
    long SendTime , GetTime ,SendACKTime,GetACKTime;
    unsigned char historyData[255][3] = {0};
    unsigned char index = 0 ;
    unsigned char ResendNum = 0 ;
    
    // 產生亂數資料
    //void RandomData(int MaxNum);
    
  public :
    unsigned char data[11] ;
    unsigned char backData[11];
    unsigned char d[6] ;
    
    // 建立建構子(節點)
    MyLoRa(unsigned char sender , unsigned char revicer , int packetSize);

    // 建立建構子(轉送)
    MyLoRa(unsigned char relay, int packetSize);

    // 內部計數器 + 1 
    void setNext();

    // 設定標頭資料
    void setHeader(unsigned char S_A);

    // 設定封包資料
    void setData(unsigned char mode , int MaxNum = 6 ,unsigned char inputData[] = {0});

    // 設定Relay位置
    void setRelay(unsigned char Address = 0x00);

    // 發送封包
    void SendPacket(bool Print);
    
    // 接收封包
    bool getPacket(bool Print);

    // 清除封包
    void claerBuffer();

    // 列印封包
    void PrintPacket(unsigned char d[] ,int Max , std::string msg);

    // 發送回應封包
    void SendACK(bool Print);

    // 接收回應封包
    bool GetACK(bool Print);

    // 建立轉送器
    void RelayBegin();

    // 設定整個封包內容
    void setPacket(unsigned char input[]);

    // 搜尋歷史資料
    bool SearchHistory(unsigned char nowPacket[]);

    // 列印出紀錄
    void PrintSeacherHistory();
    
};

#endif
