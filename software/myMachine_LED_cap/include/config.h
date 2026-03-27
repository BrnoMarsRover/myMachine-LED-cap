#pragma once

// ── WiFi ──────────────────────────────────────────────────────
#define WIFI_PASS   "12345678"
// SSID = "ksiltovka_" + hwId (sestaveno za běhu v main)

// ── Canvas (webová kresba) ────────────────────────────────────
#define CANVAS_W  320
#define CANVAS_H  240

// ── Displej ───────────────────────────────────────────────────
#define DISP_W    480
#define DISP_H    320

#define TFT_CS    5
#define TFT_DC    14
#define TFT_RST   33
#define TFT_SCK   18
#define TFT_MOSI  19

// ── Baterie ───────────────────────────────────────────────────
// Dělič napětí 22k/68k: V_adc = V_batt * 68/90
// Kalibruj tyto hodnoty podle skutečného měření ADC
#define BATT_RAW_MAX          3900   // ADC raw @ ~4.2 V (100 %)
#define BATT_RAW_MIN          3100   // ADC raw @ ~3.3 V (  0 %)
#define BATT_SAMPLE_INTERVAL_MS 1000  // vzorkování ADC každou 1 s (EMA)
#define BATT_CHECK_INTERVAL_MS  30000 // ochranný check + web update každých 30 s
#define BATT_SLEEP_RECHECK_US  60000000ULL // po 60 s znovu zkontroluj

// ── Blinkr – kalibrace osy akcelerometru ─────────────────────
// Naměřená orientace: gravitace na X, boční náklon = Y, doprodu = Z
#define TILT_THRESHOLD  2.0f  // m/s² – minimální akcelerace pro detekci náklonu
#define TILT_INVERT_Y   0     // 1 = prohoď LEFT ↔ RIGHT (pokud jsou strany prohozené)
