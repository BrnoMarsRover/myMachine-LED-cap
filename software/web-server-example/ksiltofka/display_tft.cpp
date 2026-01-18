#include "display_tft.h"

bool DisplayTFT::begin() {
  tft.init(320, 480);
  // případně: tft.init(320, 480, ST7796S_BGR);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  return true;
}

void DisplayTFT::showBootScreen() {
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ESP32 Canvas -> TFT");
}

uint16_t DisplayTFT::rgb565_from_hex(const String& hex) {
  if (hex.length() != 7 || hex[0] != '#') return ST77XX_WHITE;

  auto val = [](char c)->uint8_t {
    if (c >= '0' && c <= '9') return (uint8_t)(c - '0');
    if (c >= 'a' && c <= 'f') return (uint8_t)(10 + (c - 'a'));
    if (c >= 'A' && c <= 'F') return (uint8_t)(10 + (c - 'A'));
    return 0;
  };

  uint8_t r = (val(hex[1]) << 4) | val(hex[2]);
  uint8_t g = (val(hex[3]) << 4) | val(hex[4]);
  uint8_t b = (val(hex[5]) << 4) | val(hex[6]);

  // tvoje varianta swap G/B
  return tft.color565(r, b, g);
}

void DisplayTFT::drawThickLine(int x0, int y0, int x1, int y1, uint16_t color, int thickness) {
  if (thickness <= 1) {
    tft.drawLine(x0, y0, x1, y1, color);
    return;
  }

  int half = thickness / 2;
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  if (dx >= dy) {
    for (int o = -half; o <= half; o++) tft.drawLine(x0, y0 + o, x1, y1 + o, color);
  } else {
    for (int o = -half; o <= half; o++) tft.drawLine(x0 + o, y0, x1 + o, y1, color);
  }
}

void DisplayTFT::handleDraw(const DrawMsg& m) {
  uint16_t col = rgb565_from_hex(m.color);
  if (m.tool == "eraser") col = ST77XX_BLACK;

  if (m.type == DrawType::Clear) {
    tft.fillScreen(ST77XX_BLACK);
    penDown = false;
    return;
  }

  if (m.type == DrawType::Start) {
    penDown = true;
    lastX = m.x; lastY = m.y;

    int r = (m.thickness * 3) / 2; // *1.5
    if (r < 1) r = 1;

    tft.fillCircle(mapX(m.x), mapY(m.y), r, col);
    return;
  }

  if (m.type == DrawType::Move) {
    if (!penDown) {
      penDown = true;
      lastX = m.x; lastY = m.y;
      return;
    }

    int x0 = mapX(lastX), y0 = mapY(lastY);
    int x1 = mapX(m.x),     y1 = mapY(m.y);

    int thDisp = (m.thickness * 3) / 2;
    if (thDisp < 1) thDisp = 1;
    if (thDisp > 40) thDisp = 40;

    drawThickLine(x0, y0, x1, y1, col, thDisp);

    lastX = m.x; lastY = m.y;
    return;
  }

  if (m.type == DrawType::End) {
    penDown = false;
    return;
  }
}
