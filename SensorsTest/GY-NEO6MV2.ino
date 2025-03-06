#include <SoftwareSerial.h>
#include <TinyGPS++.h>

//ติดตั้ง Module ไว้กลางแจ้งก่อน 10-15 นาทีเพื่อทำหารปรับตำแหน่งครั้งแรกกับดาวเทียม

static const int RXPin = 3, TXPin = 2; //กำหนด Pin
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup() {
    Serial.begin(115200);
    ss.begin(GPSBaud);
    Serial.println("GPS Module Started");
}

void loop() {
    while (ss.available() > 0) {
        gps.encode(ss.read()); //อ่านค่า GPS 
    }

    // เช็คตำแหน่งของ GPS ว่าพบหรือไม่หากพบจะแสดงต่า
    if (gps.location.isValid()) {
        Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
        Serial.print("Altitude: "); Serial.println(gps.altitude.meters());
    } else {
        Serial.println("GPS not fix yet.");
    }
}
