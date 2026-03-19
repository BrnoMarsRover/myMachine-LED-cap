#include "Accelerometer.h"

Accelerometer accel;

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Starting ADXL345 test...");

    if (!accel.begin()) {
        Serial.println("ERROR: ADXL345 not found! Check wiring / I2C address.");
        while (true);
    }

    accel.printInfo();
    Serial.println("ADXL345 ready.");
}

void loop() {
    accel.update();
    Serial.printf("X: %6.2f  Y: %6.2f  Z: %6.2f  m/s2\n",
        accel.getX(), accel.getY(), accel.getZ());
    delay(200);
}
