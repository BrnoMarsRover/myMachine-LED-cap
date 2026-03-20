/* =============================================================
 * Project:   myMachine LED Cap
 * File:      myMachine_LED_cap.ino
 * Author:    Martin Kriz
 * Company:   Brno Mars Rover
 * Platform:  MH-ET LIVE ESP32 MiniKIT (ESP32-WROOM-32)
 * ============================================================= */

#include <esp_sleep.h>

#include "include/pins.h"
#include "include/config.h"
#include "include/modes.h"
#include "include/DisplayTFT.h"
#include "include/WebUI.h"
#include "include/LedStrip.h"
#include "include/Accel.h"
#include "include/Motor.h"
#include "images/kid_67.h"
#include "images/police_img.h"
#include "images/car_img.h"

// ── Globální stav ─────────────────────────────────────────────
AppMode g_mode = AppMode::DRAWING;
Mood    g_mood = Mood::SMUTNY;

DisplayTFT display;
WebUI      webui;
LedStrip   leds;
Accel      accel;
Motor      motor;

// ── Barvy nálad ───────────────────────────────────────────────
struct MoodColor { uint8_t r, g, b; };
static const MoodColor MOOD_COLORS[(int)Mood::MOOD_COUNT] = {
    {  0,   0, 255},  // SMUTNY     – modrá
    {255, 255,   0},  // VESELY     – žlutá
    {255,   0,   0},  // NASTVANY   – červená
    {255, 136,   0},  // PREKVAPENY – oranžová
    {  0, 255,   0},  // VYDESENY   – zelená
    {170,   0, 255},  // NADSENY    – fialová
};

// ── HW identifikace ───────────────────────────────────────────
uint8_t readHwId() {
    pinMode(PIN_ID_0, INPUT);
    pinMode(PIN_ID_1, INPUT);
    return (digitalRead(PIN_ID_1) << 1) | digitalRead(PIN_ID_0);
}

// ── Inicializace pinů ─────────────────────────────────────────
void initPins() {
    pinMode(PIN_M_SLEEP, OUTPUT); digitalWrite(PIN_M_SLEEP, LOW);
    pinMode(PIN_M_FAULT, INPUT);
    pinMode(PIN_M_DIR,   OUTPUT); digitalWrite(PIN_M_DIR,   LOW);
    pinMode(PIN_M_PWM,   OUTPUT); digitalWrite(PIN_M_PWM,   LOW);

    pinMode(PIN_D_RESET, OUTPUT); digitalWrite(PIN_D_RESET, HIGH);
    pinMode(PIN_D_CS,    OUTPUT); digitalWrite(PIN_D_CS,    HIGH);
    pinMode(PIN_D_DC,    OUTPUT); digitalWrite(PIN_D_DC,    LOW);
    pinMode(PIN_D_SCK,   OUTPUT); digitalWrite(PIN_D_SCK,   LOW);
    pinMode(PIN_D_MOSI,  OUTPUT); digitalWrite(PIN_D_MOSI,  LOW);

    pinMode(PIN_LED_DATA, OUTPUT); digitalWrite(PIN_LED_DATA, LOW);

    pinMode(PIN_A_INT, INPUT);
    pinMode(PIN_B_STATUS, INPUT);
}

// ── Baterie ───────────────────────────────────────────────────
uint32_t _battCheckTimer = 0;

void checkBattery() {
    if (millis() - _battCheckTimer < BATT_CHECK_INTERVAL_MS) return;
    _battCheckTimer = millis();

    int raw = analogRead(PIN_B_STATUS);
    Serial.printf("Battery raw: %d\n", raw);

    if (raw < BATT_RAW_MIN) {
        Serial.println("Battery low! Entering deep sleep.");
        leds.clearAll();
        display.clear(ST77XX_BLACK);
        esp_sleep_enable_timer_wakeup(BATT_SLEEP_RECHECK_US);
        esp_deep_sleep_start();
    }
}

// ══════════════════════════════════════════════════════════════
void setup() {
    Serial.begin(115200);

    initPins();

    // Zkontroluj baterii před spuštěním (zamezí bootloop při vybitém akumulátoru)
    int rawBatt = analogRead(PIN_B_STATUS);
    Serial.printf("Battery raw (boot): %d\n", rawBatt);
    if (rawBatt < BATT_RAW_MIN) {
        Serial.println("Battery low at boot! Deep sleep.");
        esp_sleep_enable_timer_wakeup(BATT_SLEEP_RECHECK_US);
        esp_deep_sleep_start();
    }

    // HW ID → SSID
    uint8_t hwId = readHwId();
    char ssid[24];
    snprintf(ssid, sizeof(ssid), "ksiltovka_%d", hwId);
    Serial.printf("HW ID: %d  SSID: %s\n", hwId, ssid);

    // Displej
    display.begin();

    // Boot obrázek 5 s
    display.showImage(kid_67, KID_67_W, KID_67_H);
    delay(5000);
    display.clear();

    // LED pásek – nastav výchozí náladu
    leds.begin();
    const MoodColor& mc = MOOD_COLORS[(int)g_mood];
    leds.setAll(mc.r, mc.g, mc.b);

    // Akcelerometr + motor
    if (!accel.begin()) {
        Serial.println("WARN: ADXL345 nenalezen!");
    }
    motor.begin();

    // Web server
    webui.begin(ssid, [](const DrawMsg& msg) {
        display.handleDraw(msg);
    });

    Serial.println("Setup hotov.");
}

// ══════════════════════════════════════════════════════════════

// ── SOS mód ───────────────────────────────────────────────────
AppMode _lastMode     = (AppMode)255;   // vynutí inicializaci při prvním loop()
Mood    _lastMood     = (Mood)255;

bool    _sosBlue      = true;
uint32_t _sosTimer    = 0;

void handleSos() {
    if (_lastMode != AppMode::SOS) {
        display.showImage(police_img, POLICE_IMG_W, POLICE_IMG_H);
        motor.stop();
        _sosTimer = millis();
        _sosBlue  = true;
        leds.setAll(0, 0, 255);
    }
    if (millis() - _sosTimer >= 500) {
        _sosTimer = millis();
        _sosBlue  = !_sosBlue;
        if (_sosBlue) leds.setAll(0, 0, 255);
        else          leds.setAll(255, 0, 0);
    }
}

// ── Blinkr mód ────────────────────────────────────────────────
uint32_t _blinkTimer  = 0;
bool     _blinkOn     = false;
TiltDir  _lastTiltDir = TiltDir::NONE;

void handleBlinkr() {
    if (_lastMode != AppMode::BLINKR) {
        display.showImage(car_img, CAR_IMG_W, CAR_IMG_H);
        leds.clearAll();
    }

    accel.update();
    TiltDir  dir    = accel.getTiltDir();
    int      duty   = accel.getMotorDuty();
    uint32_t period = accel.getBlinkPeriod();

    // Motor – rychlost podle náklonu, směr podle levá/pravá
    motor.stop();
    if      (dir == TiltDir::RIGHT) motor.forward(duty);
    else if (dir == TiltDir::LEFT)  motor.reverse(duty);
    motor.dutyUpdate();

    // LED blikání
    if (millis() - _blinkTimer >= period / 2) {
        _blinkTimer = millis();
        _blinkOn    = !_blinkOn;

        leds.clearAll();
        if (_blinkOn && dir != TiltDir::NONE) {
            switch (dir) {
                case TiltDir::RIGHT: leds.setSegment(Segment::RIGHT, 255, 100, 0); break;
                case TiltDir::LEFT:  leds.setSegment(Segment::LEFT,  255, 100, 0); break;
                case TiltDir::FRONT: leds.setSegment(Segment::FRONT, 255, 100, 0); break;
                default: break;
            }
        }
    }
}

// ── Smyčka ────────────────────────────────────────────────────
void loop() {
    webui.loop();
    checkBattery();

    switch (g_mode) {
        case AppMode::DRAWING: {
            bool modeEntry = (_lastMode != AppMode::DRAWING);
            bool moodChange = (g_mood != _lastMood);
            if (modeEntry) {
                motor.stop();
                display.clear(ST77XX_WHITE);
            }
            if (modeEntry || moodChange) {
                const MoodColor& mc = MOOD_COLORS[(int)g_mood];
                leds.setAll(mc.r, mc.g, mc.b);
            }
            break;
        }
        case AppMode::SOS:
            handleSos();
            break;

        case AppMode::BLINKR:
            handleBlinkr();
            break;
    }

    _lastMode = g_mode;
    _lastMood = g_mood;
}
