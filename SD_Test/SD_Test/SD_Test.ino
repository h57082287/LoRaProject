// 引入套件
#include <SPI.h>
#include <SdFat.h>

// 建立物件
SdFat SD ;
File file ;

// 建立全域變數
int sd_ss_pin = 3 ;

// 建立初始化
void setup() {
 // 初始化通訊連接埠
 Serial.begin(9600);

 // 檢測SD卡模組狀態
 while(!SD.begin(sd_ss_pin))
 {
    Serial.println("Init Failed....");
    delay(500);
 }
 Serial.println("Init OK !!!");


 // 建立寫檔測試
 file = SD.open("output.txt",FILE_WRITE);
 if(file)
 {
    file.println("Test");
    file.close();
    Serial.println("File writing OK !!!");
 }
 else
 {
   Serial.println("File writing error !!!");
 }

 // 建立讀檔測試
 file = SD.open("output.txt");
 if(file)
 {
   while(file.available())
   {
      Serial.println(file.readString());
   }
 }
 else
 {
  Serial.println("Reading Error !!!");
 }

}



// 建立迴圈
void loop() {
  // put your main code here, to run repeatedly:

}
