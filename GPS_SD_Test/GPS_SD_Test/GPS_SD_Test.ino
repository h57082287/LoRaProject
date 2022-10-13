// 引入套件
#include <SPI.h>
#include <SdFat.h>
#include<SoftwareSerial.h>


// 建立物件
SdFat SD ;
File file ;
SoftwareSerial GPS(3,4); // RX,TX

// 建立全域變數
int sd_ss_pin = 10 ;
byte data; 

void setup() {
 // 初始化通訊連接埠
 Serial.begin(9600);

 // 初始化GPS
 GPS.begin(9600);
 
 // 檢測SD卡模組狀態
 while(!SD.begin(sd_ss_pin))
 {
    Serial.println("Init Failed....");
    delay(500);
 }
 Serial.println("Init OK !!!");

}

void loop() {
  if(GPS.available())
  {
    data = GPS.read();
    Serial.write(data);
    WriteToFile(data);
  }
}


void WriteToFile(char data)
{
  // 建立寫檔測試
 file = SD.open("GPSData.txt",FILE_WRITE);
 if(file)
 {
    file.write(data);
    file.close();
 }
 else
 {
   Serial.println("File writing error !!!");
 }
}
