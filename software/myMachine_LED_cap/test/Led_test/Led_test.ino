#include "Led.h"

Led led;

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Starting NeoPixel LED strip test...");

    led.begin();
    Serial.println("LED strip initialized.");
}

void loop() {
    Serial.println("RED");
    led.setAll(255, 0, 0);
    delay(1000);

    Serial.println("GREEN");
    led.setAll(0, 255, 0);
    delay(1000);

    Serial.println("BLUE");
    led.setAll(0, 0, 255);
    delay(1000);

    Serial.println("Pixel sweep...");
    led.clear();
    for (int i = 0; i < LED_COUNT; i++) {
        led.setPixel(i, 255, 255, 255);
        delay(50);
        led.setPixel(i, 0, 0, 0);
    }
}
