/*
 * 本節點為LoRa Node 位址為 0x01
 * 本節點為主要"發送端"
 *範例封包
 * packet[12] = {0x01,0x02,0x00,0x01,0x61,0x01,0x61,0x62,0x63,0x31,0x32,0x33};
 * 封包格式       From, To,Relay,S/A, -- Seq--- ,--------------data----------
 */
// 節點資訊定義區
// =================================================================================================
byte sender = 0x01 ;
byte revicer = 0x02 ;
byte num = 0x00 ;

// 套件庫引入區
// =================================================================================================
#include <LoRa.h>
#include <SPI.h>

// 功能函數定義區
// =================================================================================================
// 發送封包
void SendPacket(byte* packetData, int Size)
{
  LoRa.beginPacket();
  LoRa.write(packetData,Size);
  LoRa.endPacket();
}

// 設置資料內容
byte* SetData(byte* packetData , int mode , byte* Data={0})
{
  byte* p ;
  
  // 模式一: 手動輸入,模式二: 亂數生成
  switch(mode)
  {
    case 1 :
      for(int i = 0 ; i < sizeof(Data) ; i++)
      {
        p[6+i] = Data[i];
      }
      break;
    case 2 : 
      p = RandomData(packetData);
      break;
  }

  return p ;
}

// 設置標頭內容
byte* setHeader(byte* packetData,byte* agrment)
{
  byte* packet = packetData;
  byte* agr = agrment;
  for(int i = 0 ; i < 6 ; i++)
  {
    packet[i] = agr[i];
  }
  return packet;
}

// 產生亂數資料
byte* RandomData(byte* data)
{
  byte* d = data ;
  // 資料範圍位置6~位置11
  for(int i = 6 ; i <= 11 ; i++)
  {
    d[i] = 22 ;//random(97,122);
    Serial.println(d[i],DEC);
    PrintData(d,12,"Check : ");
  }
  return d;
}

// 等候接收資料
byte* awaitReviceData(byte seq)
{
  byte* p ;
  while(true)
  {
    int packetSize = LoRa.parsePacket();
    // 獲取封包內容
    if(packetSize)
    {
      LoRa.readBytes(p,packetSize);
    }
    // 確認封包內容
    if(ParsePacket(p,seq))
    {
      break;
    }
  }
  return p ;
}

// 分析封包內容
bool ParsePacket(byte* packetData,byte seq)
{
  bool check = true;
  if(packetData[1] != revicer)
  {
    check = false;
  }
  if(packetData[5] != seq)
  {
    check= false; 
  }
  if(packetData[3] != 0x02)
  {
    check= false; 
  }
  return check;
}

void PrintData(byte* data , int Size ,String Msg)
{
  Serial.print(Msg);
  for(int i = 0 ; i < Size ; i ++)
  {
    Serial.print(data[i],HEX);
    Serial.print(' ');
  }
  Serial.println();
}


// 內建函數區
// =================================================================================================
void setup() {
  Serial.begin(9600);
  if(!LoRa.begin(915E6))
  {
    Serial.println("Start Fail~");
    while(1);
  }
  Serial.println("~ Lora is Begin ~");

}

void loop() {
  byte InitPacket[12]  = {0};
  byte header[5] = {sender,revicer,0x00,0x61,num};
  byte* backPacket;

  // 設定參數
  byte* buf = setHeader(InitPacket,header);
  byte* packet = SetData(buf,2);

  PrintData(packet,12,"Send Out Data :");
  
  SendPacket(packet,12);
  backPacket = awaitReviceData(num);
  num+=1;

  PrintData(backPacket,12,"Get Back Data :");

}
