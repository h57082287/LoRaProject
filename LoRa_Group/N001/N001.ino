/*
 * 本節點為LoRa Node 位址為 0x01
 *範例封包
 * packet[12] = {0x01,0x02,0x00,0x01,0x61,0x01,0x61,0x62,0x63,0x31,0x32,0x33};
 * 封包格式       From, To,Relay,S/A, -- Seq--- ,--------------data----------
 */

// 引入函數庫
#include<LoRa.h>
#include<SPI.h>

// 建立全域變數
byte packet[12];
byte* changePacket;
byte* DataBuffer;
unsigned long Time ,nowTime;

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

  // 設定本節點獨有資訊
  packet[0] = 1 ;
  packet[1] = 2 ; //RandomAddress(2,5);
  packet[2] = 0 ;
  packet[3] = 1 ;
  packet[4] = 61 ;
  

  // 設定初次發送資料
  changePacket = RandomData(packet,0);

  // 暫存資料
  DataBuffer = changePacket;

  // 檢查用
  Serial.print("Send: ");
  for(int i = 0 ; i < 12 ; i++)
  {
    Serial.print(changePacket[i]);
    Serial.print(' ');
  }
  Serial.println("");

  // 發送資料
  PacketOut(changePacket);
  Time = millis();
}

void loop() {
  static byte num  = 0;
  Serial.println(num);
  // 檢測是否或的資料
  int PacketSize = LoRa.parsePacket();
  nowTime = millis();
  if(PacketSize > 0)
  {
    byte response[PacketSize];
    LoRa.readBytes(response,PacketSize);
    
    // 檢查用
    Serial.print("Get:  ");
    for(int i = 0 ; i < 12 ; i++)
    {
      Serial.print(response[i]);
      Serial.print(' '); 
    }
    Serial.println("");
    
    if(response[3] == 2) // 確認是否為ACK
    {
      //Serial.println(response[5]);
      //Serial.println(num);
      // 確認裝置編號及成功代碼
      if(response[5] == num && response[4] == 61)
      {
        Serial.println("Success Send ~");


        // 序列設定
        num++ ;
        
        // 溢位歸零
        if (num == 4)
        {
          num = 1 ;
        }
        
        // 設定新的發送資料 : 發送者、收件者、S/A、
        changePacket = RandomData(packet,num);
        
        // 延遲送信
        delay(3000);

        // 暫存資料用
        DataBuffer = changePacket ;
        
        // 發送資料
        Serial.println(DataBuffer[5]);
        PacketOut(changePacket);

        //紀錄時間
        Time = millis();

        // 檢查用
        Serial.print("Send: ");
        for(int i = 0 ; i < 12 ; i++)
        {
          Serial.print(changePacket[i]);
          Serial.print(' ');
        }
        Serial.println("");
        
      }
    }
  }
  else if ((nowTime - Time) > 10000)
  {
    PacketOut(DataBuffer);
    // 檢查用
    Serial.print("ReSend: ");
    for(int i = 0 ; i < 12 ; i++)
    {
      Serial.print(DataBuffer[i]);
      Serial.print(' ');
    }
    Serial.println("");
    Time = millis();
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
  data[0] = 1 ;
  data[1] = 2 ; //  RandomAddress(2,3);
  data[3] = 1 ;
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
