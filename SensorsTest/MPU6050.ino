#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 connection failed!"); //แสดงค่าเมื่อไม่สามารถเริ่มต้นระบบได้
        while (1);
    }
}

void loop() {
    int16_t ax, ay, az; 
    int16_t gx, gy, gz;

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    Serial.print("Accel X: "); Serial.print(ax);
    Serial.print(" | Y: "); Serial.print(ay);
    Serial.print(" | Z: "); Serial.println(az);

    delay(1000); //แสดงค่าความเร่งในแกน X Y Z
}
