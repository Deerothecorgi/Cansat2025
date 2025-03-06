#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <math.h>
#include <SPI.h>
#include <SD.h>
#include <LoRa.h>
#include <OV7670.h>

// กำหนด Pin
#define MQ135_NH3_Pin A0 // Pin ของ MQ-135 เพื่อวัด NH3 
#define MQ135_NOx_Pin A1 // Pin ของ MQ-135 เพื่อวัด NOx
#define RXPin 3 // Pin RX ของ GPS
#define TXPin 2 // Pin TX ของ GPS
#define GPSBaud 9600 // Baud Rate ของ GPS
#define LORA_SS 10 // Pin SS ของ Lora Module
#define LORA_RST 9 // Pin Reset ของ Lora Module
#define LORA_DIO0 2 // Pin DIO0 ของ Lora Module
#define SD_CS 4 // Pin ของ SD Card

MPU6050 mpu;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
OV7670 cam;

float Ro_NH3 = 10.0; // ค่า Ro ของ NH3 ที่ปรับเทียบแล้ว
float Ro_NOx = 10.0; // ค่า Ro ของ NH3 ที่ปรับเทียบแล้ว

void setup() {
    Serial.begin(115200);
    Wire.begin();
    ss.begin(GPSBaud);

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(915E6)) Serial.println("LoRa init failed!"); // เรื่มการทำงานของ LoRa และแสดงผลหากเกิดข้อผิดพลาด
    if (!SD.begin(SD_CS)) Serial.println("SD Card init failed!"); // เรื่มการทำงานของ SD card และแสดงผลหากเกิดข้อผิดพลาด

    mpu.initialize(); 
    if (!mpu.testConnection()) Serial.println("MPU6050 connection failed!"); // เรื่มการทำงานของ MPU6050 ละแสดงผลหากเกิดข้อผิดพลาด

    Ro_NH3 = calibrateSensor(MQ135_NH3_Pin); // ปรับค่า Ro ของ NH3
    Ro_NOx = calibrateSensor(MQ135_NOx_Pin); // ปรับค่า Ro ของ NOx

    cam.init(); // เริ้มการทำงานของกล้อง OV7670
    cam.setRes(QVGA);
    cam.setColorSpace(YUV422);
}

void loop() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); 
    float NH3_ppm = getGasPPM(MQ135_NH3_Pin, Ro_NH3, 116.6, -2.76); // อ่านค่าความเข้มข่นของ NH3
    float NOx_ppm = getGasPPM(MQ135_NOx_Pin, Ro_NOx, 220.0, -2.3); // อ่านค่าความเข้มข่นของ NOx

    while (ss.available() > 0) gps.encode(ss.read()); // อ่านค่า GPS
    String data = "";

    if (gps.location.isValid()) { //หากพบตำแหน่งจากดาวเทียม
        data += "Lat:" + String(gps.location.lat(), 6) + " ";
        data += "Lng:" + String(gps.location.lng(), 6) + " ";
        data += "Alt:" + String(gps.altitude.meters()) + "m ";
        data += "Sat:" + String(gps.satellites.value()) + " ";
    }

    data += "NH3:" + String(NH3_ppm) + "ppm ";
    data += "NOx:" + String(NOx_ppm) + "ppm ";
    data += "AccelX:" + String(ax) + " ";
    data += "AccelY:" + String(ay) + " ";
    data += "AccelZ:" + String(az);
    
    LoRa.beginPacket();
    LoRa.print(data); // ส่งข้อมูลผ่าน Lora Module
    LoRa.endPacket();
    Serial.println("Sent: " + data);
    delay(2000);
}

// ปรับเทียบโดยการคำนวนค่า Ro ในอากาศบริสุทธิ์
float calibrateSensor(int pin) {
    float val = 0;
    for (int i = 0; i < 100; i++) {
        val += analogRead(pin);
        delay(50);
    }
    val = val / 100;  // เฉลี่ยค่าที่อ่าน
    float Rs_air = (1023.0 / val) - 1.0;  // คำนวนค่า resistance ในอากาศบริสุทธิ์
    return Rs_air / 3.6;  // 3.6 คืออัตราส่วนตาม MQ-135 DataSheet
}

// เปลี่ยนเป็นค่า ppm ตามความ curve ของแก๊ซ
float getGasPPM(int pin, float Ro, float A, float B) {
    int sensorValue = analogRead(pin);
    float Rs = (1023.0 / sensorValue) - 1.0;
    Rs = Ro / Rs;
    return A * pow(Rs, B);  // คำนวนค่า Sensitivity
}
