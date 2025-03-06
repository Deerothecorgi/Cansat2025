#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define LORA_SS 5 // Pin SS ของ Lora Module
#define LORA_RST 14 // Pin Reset ของ Lora Module
#define LORA_DIO0 2 // Pin DIO0 ของ Lora Module

const char* ssid = "XXXXXXXX"; // SSID ของ WiFi 
const char* password = "XXXXXXXX"; // รหัสของ WiFi

const char* netpieHost = "broker.netpie.io"; // URL ของ NETPIE
const char* netpieToken = "XXXXXXXX"; // Token ของ NETPIE
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password); // เชื่อมต่อกับ WiFi

    while (WiFi.status() != WL_CONNECTED) delay(500);
    client.setServer(netpieHost, 1883); // เชื่อมต่อกับ NETPIE MQTT broker
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(915E6)) Serial.println("LoRa init failed!"); // เรื่มการทำงานของ LoRa
}

void loop() {
    int packetSize = LoRa.parsePacket(); // ตรวจสอบว่า Lora พบข้อมูลหรือไม่
    if (packetSize) {
        String receivedData = "";
        while (LoRa.available()) receivedData += (char)LoRa.read(); // อ่านข้อมูลของ LoRa 
        Serial.println("Received: " + receivedData);
        if (!client.connected()) client.connect("ESP32_Client", netpieToken, "");
        client.publish("@msg/data", receivedData.c_str()); // ส่งข้อมูลที่ได้ไปที่ to NETPIE
    }
    delay(1000);
}
