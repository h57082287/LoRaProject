// 引入套件庫
#include<SoftwareSerial.h>

SoftwareSerial GPS(3,4); // RX,TX

// 建立全域變數
byte data; 
String s ;
char b = "bb";
char c = "c";

void setup() {
 Serial.begin(9600);
 GPS.begin(9600);
}

void loop() {
  if(GPS.available())
  {
    data = GPS.read();
    Serial.print(char(data));
  }
}
