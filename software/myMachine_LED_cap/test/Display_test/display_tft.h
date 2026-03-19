#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>

#include "config.h"
#include "ws_protocol.h"

class DisplayTFT {
public:
  bool begin();
  void showBootScreen();
  void handleDraw(const DrawMsg& m);

private:
  Adafruit_ST7796S tft = Adafruit_ST7796S(TFT_CS, TFT_DC, TFT_RST);

  bool penDown = false;
  int lastX = 0;
  int lastY = 0;

  static inline int mapX(int x) { return (x * DISP_W) / CANVAS_W; }
  static inline int mapY(int y) { return (y * DISP_H) / CANVAS_H; }

  uint16_t rgb565_from_hex(const String& hex);
  void drawThickLine(int x0, int y0, int x1, int y1, uint16_t color, int thickness);
};
