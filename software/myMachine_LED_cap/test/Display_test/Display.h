/* =============================================================
 * Project:   myMachine LED Cap
 * File:      Display.h
 * Author:    Martin Kriz
 * Company:   Brno Mars Rover
 * -------------------------------------------------------------
 * Description:
 *   Display class – ST7796S over hardware SPI.
 *   Pins: RESET=33, DC=14, CS=5, SCK=18, MOSI=19
 *   Library: Adafruit_ST7796S + Adafruit_GFX
 *   Resolution: 320x480 (landscape: 480x320)
 * ============================================================= */

#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>

constexpr int D_RESET_PIN = 33;
constexpr int D_DC_PIN    = 14;
constexpr int D_CS_PIN    = 5;
constexpr int D_SCK_PIN   = 18;
constexpr int D_MOSI_PIN  = 19;

class Display {
public:
    Display() : _tft(D_CS_PIN, D_DC_PIN, D_RESET_PIN) {}

    void begin() {
        SPI.begin(D_SCK_PIN, -1, D_MOSI_PIN, D_CS_PIN);
        _tft.init(320, 480);
        _tft.invertDisplay(true);
        _tft.setRotation(1);   // landscape 480x320
        clear();
    }

    void clear(uint16_t color = ST77XX_BLACK) {
        _tft.fillScreen(color);
    }

    void printText(const char* text, int x, int y,
                   uint8_t size = 2, uint16_t color = ST77XX_WHITE) {
        _tft.setTextColor(color);
        _tft.setTextSize(size);
        _tft.setCursor(x, y);
        _tft.print(text);
    }

    void drawRect(int x, int y, int w, int h, uint16_t color) {
        _tft.drawRect(x, y, w, h, color);
    }

    void fillRect(int x, int y, int w, int h, uint16_t color) {
        _tft.fillRect(x, y, w, h, color);
    }

    Adafruit_ST7796S& tft() { return _tft; }

private:
    Adafruit_ST7796S _tft;
};
