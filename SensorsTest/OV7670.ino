#include <Wire.h>
#include <OV7670.h>

// ใช่ Digital Pins และเนื่องจากมี Digital Pin บน Arduino Uno ไม่พอจึงสามารถใช้ Analog Pin แทนได้
const int VSYNC = 2;
const int HREF = 3;
const int PCLK = 13;
const int XCLK = 9;
const int D0 = 8;
const int D1 = 7;
const int D2 = 6;
const int D3 = 5;
const int D4 = 4;
const int D5 = A3;
const int D6 = A2;
const int D7 = A1;

OV7670 cam;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // กำหนด Pin ของกล้อง
  pinMode(VSYNC, INPUT);
  pinMode(HREF, INPUT);
  pinMode(PCLK, INPUT);
  pinMode(XCLK, OUTPUT);
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);

  // เริ่มการทำงานของกล้อง
  cam.init();
  cam.setRes(QVGA);
  cam.setColorSpace(YUV422);
  Serial.println("Camera initialized.");
}

void loop() {
  cam.waitForVsync();
  cam.readFrame();
  Serial.println("Frame captured.");

  // ส่งข้อมูลไปยัง Serial โดยรับข้อมูลได้จากโปรแกรม CoolTerm โดยปรับค่า Baud rate ที่ 115200 
  for (int i = 0; i < cam.frameLength; i++) {
    Serial.write(cam.frameBuffer[i]);
  }

  Serial.println("Image data sent over Serial.");
  delay(5000); // ถ่ายภาพทุกๆ 5 วินาที
}
