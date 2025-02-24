#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Servo.h>
#include <SD.h>

 // ----- Pin Definitions for Arduino Uno -----
#define LORA_SS   10     // LoRa module Chip Select
#define LORA_RST  9      // LoRa module Reset
#define LORA_DIO0 2      // LoRa module DIO0

// GPS module using SoftwareSerial
SoftwareSerial gpsSerial(4, 3);  // GPS: RX = 4, TX = 3
TinyGPSPlus gps;

// Servo for shock chord deployment
Servo shockServo;
const int servoPin = 6;
bool chordDeployed = false;

// MQ-135 Gas Sensor (Analog)
const int gasSensorPin = A0;

// Dummy Accelerometer values (replace with sensor’s library)
float ax = 0, ay = 0, az = 0;

// SD Card module chip select pin
const int SD_CS = 8;

// (OV7670 Camera Module)
// Note: The OV7670 typically requires many signals (e.g., XCLK, D0-D7, HREF, VSYNC, PCLK).
// Here we assume a library/firmware handles the low-level interface.

void setup() {
    Serial.begin(9600);
    while (!Serial);

    // Initialize SD card for image storage
    if (!SD.begin(SD_CS)) {
        Serial.println("SD card initialization failed!");
    }
    else {
        Serial.println("SD card initialized.");
    }

    // Initialize LoRa module via SPI
    SPI.begin();
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(915E6)) {  // Frequency: 915 MHz (adjust as needed)
        Serial.println("LoRa initialization failed!");
        while (1);
    }
    Serial.println("LoRa initialized.");

    // Initialize GPS
    gpsSerial.begin(9600);

    // Initialize servo (start at 0°)
    shockServo.attach(servoPin);
    shockServo.write(0);

    // Initialize I2C (for accelerometer, etc.)
    Wire.begin();
}

void loop() {
    // 1. Read dummy accelerometer values (replace with actual sensor reads)
    readAccelerometer();

    // 2. Read gas sensor value
    int gasValue = analogRead(gasSensorPin);

    // 3. Process incoming GPS data
    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }
    double latitude = gps.location.isValid() ? gps.location.lat() : 0.0;
    double longitude = gps.location.isValid() ? gps.location.lng() : 0.0;
    double altitude = gps.altitude.isValid() ? gps.altitude.meters() : 0.0;

    // 4. Deploy shock chord if altitude >= 500m and not already deployed
    if (!chordDeployed && altitude >= 500) {
        shockServo.write(90);  // Adjust angle to trigger  mechanism
        chordDeployed = true;
        Serial.println("Shock chord deployed!");
    }

    // 5. Capture an image from the OV7670 (simulated) after deployment
    if (chordDeployed) {
        captureImage();
    }

    // 6. Create a data packet to send via LoRa
    String dataPacket = "LAT:" + String(latitude, 6) +
        ",LON:" + String(longitude, 6) +
        ",ALT:" + String(altitude, 2) +
        ",AX:" + String(ax, 2) +
        ",AY:" + String(ay, 2) +
        ",AZ:" + String(az, 2) +
        ",GAS:" + String(gasValue);

    LoRa.beginPacket();
    LoRa.print(dataPacket);
    LoRa.endPacket();
    Serial.println("Data sent: " + dataPacket);

    delay(1000);  // Adjust as necessary
}

void readAccelerometer() {
    // Dummy accelerometer values (simulate readings)
    ax = random(-100, 100) / 100.0;
    ay = random(-100, 100) / 100.0;
    az = random(900, 1100) / 100.0;  // Approximately 1g on Z-axis
}

void captureImage() {
    // --- Placeholder for OV7670 Image Capture ---
    // In a real scenario, you would use an OV7670 library to grab a frame and save it.
    Serial.println("Capturing image from OV7670...");

    // Create a unique filename based on millis()
    String filename = "IMG" + String(millis()) + ".bmp";
    File imgFile = SD.open(filename, FILE_WRITE);
    if (imgFile) {
        // Simulate writing image data (replace with actual image capture code)
        imgFile.println("This is a dummy image captured from OV7670.");
        imgFile.close();
        Serial.println("Image saved as " + filename);
    }
    else {
        Serial.println("Error opening file " + filename);
    }
}
