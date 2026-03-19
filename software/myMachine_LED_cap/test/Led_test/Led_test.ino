#include "Led.h"

Led led;

// ── Helpers ───────────────────────────────────────────────────

void colorWipe(uint8_t r, uint8_t g, uint8_t b, int wait) {
    for (int i = 0; i < LED_COUNT; i++) {
        led.setPixel(i, r, g, b);
        delay(wait);
    }
}

void theaterChase(uint8_t r, uint8_t g, uint8_t b, int wait, int cycles) {
    for (int c = 0; c < cycles; c++) {
        for (int offset = 0; offset < 3; offset++) {
            led.clear();
            for (int i = offset; i < LED_COUNT; i += 3) {
                led.setPixel(i, r, g, b);
            }
            led.show();
            delay(wait);
        }
    }
}

void rainbow(int wait, int cycles) {
    for (int c = 0; c < cycles; c++) {
        for (long hue = 0; hue < 65536; hue += 512) {
            led.fillRainbow(hue, 65536 / LED_COUNT);
            delay(wait);
        }
    }
}

void theaterChaseRainbow(int wait, int cycles) {
    long hue = 0;
    for (int c = 0; c < cycles; c++) {
        for (int offset = 0; offset < 3; offset++) {
            led.clear();
            for (int i = offset; i < LED_COUNT; i += 3) {
                uint32_t color = led.strip().gamma32(led.strip().ColorHSV(hue + (i * 65536L / LED_COUNT)));
                led.strip().setPixelColor(i, color);
            }
            led.show();
            hue += 2048;
            delay(wait);
        }
    }
}

void breathe(uint8_t r, uint8_t g, uint8_t b, int cycles) {
    for (int c = 0; c < cycles; c++) {
        for (int br = 0; br <= 50; br++) {
            led.setBrightness(br);
            led.setAll(r, g, b);
            delay(15);
        }
        for (int br = 50; br >= 0; br--) {
            led.setBrightness(br);
            led.setAll(r, g, b);
            delay(15);
        }
    }
    led.setBrightness(50);
}

void scanner(uint8_t r, uint8_t g, uint8_t b, int wait, int cycles) {
    for (int c = 0; c < cycles; c++) {
        for (int i = 0; i < LED_COUNT; i++) {
            led.clear();
            if (i > 0)         led.setPixel(i - 1, r / 4, g / 4, b / 4);
            led.setPixel(i, r, g, b);
            if (i < LED_COUNT - 1) led.setPixel(i + 1, r / 4, g / 4, b / 4);
            led.show();
            delay(wait);
        }
        for (int i = LED_COUNT - 1; i >= 0; i--) {
            led.clear();
            if (i < LED_COUNT - 1) led.setPixel(i + 1, r / 4, g / 4, b / 4);
            led.setPixel(i, r, g, b);
            if (i > 0)         led.setPixel(i - 1, r / 4, g / 4, b / 4);
            led.show();
            delay(wait);
        }
    }
}

// ── Setup ─────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("NeoPixel demo start");
    led.begin();
}

// ── Loop ──────────────────────────────────────────────────────

void loop() {
    Serial.println("1. Color wipe");
    colorWipe(255, 0,   0,   8);
    colorWipe(0,   255, 0,   8);
    colorWipe(0,   0,   255, 8);
    colorWipe(0,   0,   0,   4);

    Serial.println("2. Theater chase");
    theaterChase(255, 0,   0,   60, 5);
    theaterChase(0,   255, 0,   60, 5);
    theaterChase(0,   0,   255, 60, 5);

    Serial.println("3. Rainbow");
    rainbow(5, 2);

    Serial.println("4. Theater chase rainbow");
    theaterChaseRainbow(60, 3);

    Serial.println("5. Breathe");
    breathe(255, 0,   0,   2);
    breathe(0,   0,   255, 2);

    Serial.println("6. Scanner");
    scanner(0, 200, 255, 18, 2);

    led.clear();
    led.show();
    delay(500);
}
