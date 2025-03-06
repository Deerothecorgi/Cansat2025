#include <SPI.h>
#include <SD.h>

const int chipSelect = 10; // Pin ของการต่อ SD Card

void setup() {
  Serial.begin(9600);

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card not detected."); // SD card ไม่ถูกพบ
  } else {
    Serial.println("SD card detected."); // SD card ถูกพบ
  }
}

void loop() {
  //ตรวจสอบการเสียบ SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card removed."); // SD card ถูกนำออก
  } else {
    Serial.println("SD card is still present."); // SD card ถูกนำเข้า
  }
  
  delay(2000); // ตรวจสอบสถานะทุกๆ 2 วินาที
}
