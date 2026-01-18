#pragma once
#include <Arduino.h>

// ===== TFT piny =====
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

// Displej: 320x480 nativ, po rotation(1) = 480x320
static const int DISP_W = 480;
static const int DISP_H = 320;

// Web canvas: LANDSCAPE 320x240 (logické souřadnice)
static const int CANVAS_W = 320;
static const int CANVAS_H = 240;

// ===== WiFi AP =====
static const char* WIFI_SSID = "ESP32-AP";
static const char* WIFI_PASS = "12345678";
