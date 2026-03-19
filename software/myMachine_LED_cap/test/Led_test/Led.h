/* =============================================================
 * Project:   myMachine LED Cap
 * File:      Led.h
 * Author:    Martin Kriz
 * Company:   Brno Mars Rover
 * -------------------------------------------------------------
 * Description:
 *   LED strip class – WS2812B via Adafruit NeoPixel.
 *   Pin: DATA=4
 *   Library: Adafruit NeoPixel
 * ============================================================= */

#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

constexpr int LED_DATA_PIN = 4;
constexpr int LED_COUNT    = 172;   // TODO: adjust to actual strip length

class Led {
public:
    Led() : _strip(LED_COUNT, LED_DATA_PIN, NEO_GRB + NEO_KHZ800) {}

    void begin() {
        _strip.begin();
        _strip.setBrightness(50);
        clear();
    }

    void clear() {
        _strip.clear();
        _strip.show();
    }

    void setAll(uint8_t r, uint8_t g, uint8_t b) {
        _strip.fill(_strip.Color(r, g, b));
        _strip.show();
    }

    void setPixel(int index, uint8_t r, uint8_t g, uint8_t b) {
        if (index < 0 || index >= LED_COUNT) return;
        _strip.setPixelColor(index, _strip.Color(r, g, b));
        _strip.show();
    }

    void setBrightness(uint8_t brightness) {
        _strip.setBrightness(brightness);
        _strip.show();
    }

    void show() {
        _strip.show();
    }

    void fillRainbow(long firstHue, long hueStep) {
        for (int i = 0; i < LED_COUNT; i++) {
            uint32_t color = _strip.gamma32(_strip.ColorHSV(firstHue + i * hueStep));
            _strip.setPixelColor(i, color);
        }
        _strip.show();
    }

    Adafruit_NeoPixel& strip() { return _strip; }

private:
    Adafruit_NeoPixel _strip;
};
