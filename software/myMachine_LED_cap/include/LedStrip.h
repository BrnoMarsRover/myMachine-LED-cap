#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Segmenty pásku (0-based indexy)
// right: 0–27 a 83–110
// left:  28–82
// front: 111–169

constexpr int LED_PIN   = 4;
constexpr int LED_NUM   = 172;

enum class Segment { RIGHT, LEFT, FRONT, ALL };

class LedStrip {
public:
    LedStrip() : _strip(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800) {}

    void begin() {
        _strip.begin();
        _strip.setBrightness(50);
        clearAll();
    }

    void clearAll() {
        _strip.clear();
        _strip.show();
    }

    void setSegment(Segment seg, uint8_t r, uint8_t g, uint8_t b) {
        uint32_t color = _strip.Color(r, g, b);
        switch (seg) {
            case Segment::RIGHT:
                for (int i = 0;   i <= 27;  i++) _strip.setPixelColor(i, color);
                for (int i = 83;  i <= 110; i++) _strip.setPixelColor(i, color);
                break;
            case Segment::LEFT:
                for (int i = 28;  i <= 82;  i++) _strip.setPixelColor(i, color);
                break;
            case Segment::FRONT:
                for (int i = 111; i <= 169; i++) _strip.setPixelColor(i, color);
                break;
            case Segment::ALL:
                _strip.fill(color);
                break;
        }
        _strip.show();
    }

    void setAll(uint8_t r, uint8_t g, uint8_t b) {
        setSegment(Segment::ALL, r, g, b);
    }

private:
    Adafruit_NeoPixel _strip;
};
