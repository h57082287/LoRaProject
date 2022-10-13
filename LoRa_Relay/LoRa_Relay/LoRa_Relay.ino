// 引入函式庫
#include<LoRa.h>

// 建立LoRa物件
LoRaClass R ;

// 建立全域變數 
String SerialNumber = "R001" ;

// 初始化設定區
void setup() {
  Serial.begin(9600);
  if(!R.begin(915E6))
  {
    Serial.println("Init Fail ~");
  }
  else
  {
    Serial.println("Init OK ~");
  }
}

// 主程式運行區
void loop() {
  int Size = R.parsePacket();
  if(Size)
  {
    while(R.available())
    {
      String Data = R.readString();
      Serial.println("收到的資料:" + Data + ",信號強度:" + R.rssi());
      SendPacket(Data);
    }
  }
}

/*
// 傳送函式(節點用)
bool SendPacket(String out_Data,String To)
{
   R.beginPacket();
   //D 傳送的內容 , S 訊號強度 , T 傳給哪個節點
   R.println("D" + out_Data + "S" + L.rssi() + "T" + To);
   R.endPacket();
}
*/

// SD 儲存程式
void SeveData(String data)
{
  
}

// 傳送函式(轉送器用)
void SendPacket(String out_Data)
{
   R.beginPacket();
   R.println("F"+SerialNumber+out_Data);
   R.endPacket();
}
