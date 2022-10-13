/*
 * 本節點為LoRa Node 位址為 0x02
 *範例封包
 * packet[12] = {0x01,0x02,0x00,0x01,0x61,0x01,0x61,0x62,0x63,0x31,0x32,0x33};
 * 封包格式       From, To,Relay,S/A, -- Seq--- ,--------------data----------
 */

// 引入函數庫
#include<LoRa.h>
#include<SPI.h>

// 建立全域變數
byte packet[12];

// 初始化設定
void setup() {
  // 啟動序列視窗
  Serial.begin(9600);

  // 判斷LoRa初始化情形
  if(!LoRa.begin(915E6))
  {
    Serial.println("Start Fail~");
    while(1);
  }
  Serial.println("LoRa Start");
}

void loop() {
  // 檢測是否或的資料
  int PacketSize = LoRa.parsePacket();
  if(PacketSize > 0)
  {
    byte response[PacketSize];
    LoRa.readBytes(response,PacketSize);
    if(response[1] == 2)
    {
        Serial.println("Success Resvice ~");
        byte* newResponse;
        // 設定回應資料
        newResponse = setBackData(response);
        // 發送資料
        PacketOut(newResponse);
    }
  }
}


void PacketOut(byte* data)
{
 
  LoRa.beginPacket();
  LoRa.write(data,12);
  LoRa.endPacket();
}

// 產生亂數資料
byte* RandomData(byte* data,byte num)
{
  byte* d = data ;
  data[5] = num ;
  // 資料範圍位置6~位置11
  for(int i = 6 ; i <= 11 ; i++)
  {
    data[i] = random(97,122);
  }
  return d;
}

// 生成亂數接收者
byte RandomAddress(int Star , int End)
{
  return random(Star,End);
}

// 設定回送資料
byte* setBackData(byte* data)
{
  byte* newData = data ;
  byte backAddr = newData[0];
  newData[0] = 2 ;
  newData[1] = backAddr ;
  newData[4] = 61 ;
  newData[5] = 2 ;
  return newData;
}
