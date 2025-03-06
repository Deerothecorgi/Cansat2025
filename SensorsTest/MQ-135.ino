#include <math.h>

#define MQ135_NH3_Pin A0  //กำหนด Pin ของ NH3
#define MQ135_NOx_Pin A1  //กำหนด Pin ของ NOx

float Ro_NH3 = 10.0;  // ค่า Ro ของ NH3 ที่ปรับเทียบแล้ว
float Ro_NOx = 10.0;  // ค่า Ro ของ NOx ที่ปรับเทียบแล้ว

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

// เปลี่ยนเป็นค่า ppm ตามความโค้งของแก๊ซ
float getGasPPM(int pin, float Ro, float A, float B) {
    int sensorValue = analogRead(pin);
    float Rs = (1023.0 / sensorValue) - 1.0;
    Rs = Ro / Rs;
    return A * pow(Rs, B);  // คำนวนค่า Sensitivity
}

void setup() {
    Serial.begin(9600);
    Serial.println("Calibrating Sensors...");

    Ro_NH3 = calibrateSensor(MQ135_NH3_Pin);
    Ro_NOx = calibrateSensor(MQ135_NOx_Pin);

    Serial.print("Calibrated Ro for NH3: "); Serial.println(Ro_NH3);
    Serial.print("Calibrated Ro for NOx: "); Serial.println(Ro_NOx);
}

void loop() {
    float NH3_ppm = getGasPPM(MQ135_NH3_Pin, Ro_NH3, 116.6020682, -2.769034857);
    float NOx_ppm = getGasPPM(MQ135_NOx_Pin, Ro_NOx, 220.0, -2.3);

    Serial.print("NH3 (Ammonia) PPM: ");
    Serial.println(NH3_ppm);

    Serial.print("NOx (Nitrogen Oxides) PPM: ");
    Serial.println(NOx_ppm);

    delay(2000);
}
