#include "include/DisplayTFT.h"

bool DisplayTFT::begin() {
    SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
    tft.init(320, 480);
    tft.invertDisplay(true);
    tft.setRotation(1);   // landscape 480x320
    tft.fillScreen(ST77XX_BLACK);
    return true;
}

void DisplayTFT::showImage(const uint16_t* img, uint16_t w, uint16_t h) {
    tft.fillScreen(ST77XX_BLACK);
    int x = (DISP_W - w) / 2;
    int y = (DISP_H - h) / 2;
    tft.drawRGBBitmap(x, y, img, w, h);
}

void DisplayTFT::clear(uint16_t color) {
    tft.fillScreen(color);
    penDown = false;
    _lastHorizonDy = 32767;
}

uint16_t DisplayTFT::rgb565_from_hex(const String& hex) {
    if (hex.length() != 7 || hex[0] != '#') return ST77XX_WHITE;

    auto val = [](char c) -> uint8_t {
        if (c >= '0' && c <= '9') return (uint8_t)(c - '0');
        if (c >= 'a' && c <= 'f') return (uint8_t)(10 + (c - 'a'));
        if (c >= 'A' && c <= 'F') return (uint8_t)(10 + (c - 'A'));
        return 0;
    };

    uint8_t r = (val(hex[1]) << 4) | val(hex[2]);
    uint8_t g = (val(hex[3]) << 4) | val(hex[4]);
    uint8_t b = (val(hex[5]) << 4) | val(hex[6]);

    return tft.color565(b, g, r);
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

void DisplayTFT::showConnectInfo(const char* ssid, const char* pass, const char* ip) {
    tft.fillScreen(ST77XX_BLACK);

    // Nadpis
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(3);
    tft.setCursor(10, 12);
    tft.print("Pripojte se:");

    // WiFi SSID
    tft.setTextColor(0x07FF);  // cyan
    tft.setTextSize(2);
    tft.setCursor(10, 60);
    tft.print("WiFi: ");
    tft.setTextSize(3);
    tft.setCursor(10, 82);
    tft.print(ssid);

    // Heslo
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 130);
    tft.print("Heslo: ");
    tft.setTextColor(0x07FF);
    tft.setTextSize(3);
    tft.setCursor(10, 152);
    tft.print(pass);

    // URL
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 205);
    tft.print("Webovy prohlizec:");
    tft.setTextColor(0x07FF);
    tft.setTextSize(3);
    tft.setCursor(10, 228);
    tft.print("http://");
    tft.print(ip);
}

void DisplayTFT::showBatteryLow() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(4);
    int16_t x1, y1;
    uint16_t tw, th;
    tft.getTextBounds("Vybita baterie", 0, 0, &x1, &y1, &tw, &th);
    tft.setCursor((DISP_W - tw) / 2, (DISP_H - th) / 2);
    tft.print("Vybita baterie");
}

void DisplayTFT::drawHorizonLine(int half_dy) {
    if (half_dy == _lastHorizonDy) return;  // beze změny – nič nekreslit

    const int THICK = 5;
    const int CY    = DISP_H / 2;  // 160

    // Smazat předchozí čáru (kreslíme přes ni černou, o 2 px silnější)
    if (_lastHorizonDy != 32767) {
        int oy0 = constrain(CY + _lastHorizonDy, 0, DISP_H - 1);
        int oy1 = constrain(CY - _lastHorizonDy, 0, DISP_H - 1);
        drawThickLine(0, oy0, DISP_W - 1, oy1, ST77XX_BLACK, THICK + 2);
    }

    // Nakreslit novou čáru žlutě
    int y0 = constrain(CY + half_dy, 0, DISP_H - 1);
    int y1 = constrain(CY - half_dy, 0, DISP_H - 1);
    drawThickLine(0, y0, DISP_W - 1, y1, 0xFFE0, THICK);  // žlutá

    _lastHorizonDy = half_dy;
}

void DisplayTFT::handleDraw(const DrawMsg& m) {
    uint16_t col = rgb565_from_hex(m.color);
    if (m.tool == "eraser") col = ST77XX_WHITE;

    if (m.type == DrawType::Clear) {
        tft.fillScreen(ST77XX_WHITE);
        penDown = false;
        return;
    }

    if (m.type == DrawType::Start) {
        penDown = true;
        lastX = m.x; lastY = m.y;
        int r = (m.thickness * 3) / 2;
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
        int x1 = mapX(m.x),   y1 = mapY(m.y);
        int thDisp = (m.thickness * 3) / 2;
        if (thDisp < 1)  thDisp = 1;
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
