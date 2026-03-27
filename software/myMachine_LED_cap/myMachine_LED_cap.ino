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
float    g_battFiltered   = 0.0f;  // EMA filtrovaná ADC hodnota
uint32_t _battSampleTimer = 0;     // timer vzorkování (1 s)
uint32_t _battCheckTimer  = 0;     // timer ochranného checku (30 s)

// Naprimuje EMA filtr průměrem z N vzorků
void primeBattFilter(int samples = 10) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(PIN_B_STATUS);
        delay(5);
    }
    g_battFiltered = (float)(sum / samples);
}

void checkBattery() {
    uint32_t now = millis();

    // Vzorkování každou 1 s – aktualizuj EMA filtr
    if (now - _battSampleTimer >= BATT_SAMPLE_INTERVAL_MS) {
        _battSampleTimer = now;
        int raw = analogRead(PIN_B_STATUS);
        // EMA: alpha = 0.1 (delší časová konstanta díky hustšímu vzorkování)
        g_battFiltered = 0.1f * raw + 0.9f * g_battFiltered;
        Serial.printf("Battery raw: %d  filtered: %.0f\n", raw, g_battFiltered);
    }

    // Ochranný check každých 30 s
    if (now - _battCheckTimer >= BATT_CHECK_INTERVAL_MS) {
        _battCheckTimer = now;
        if (g_battFiltered < BATT_RAW_MIN) {
            Serial.println("Battery low! Entering deep sleep.");
            leds.clearAll();
            display.showBatteryLow();
            delay(2000);
            esp_sleep_enable_timer_wakeup(BATT_SLEEP_RECHECK_US);
            esp_deep_sleep_start();
        }
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

    // Naprimuj EMA filtr (10 vzorků = ustálená počáteční hodnota)
    primeBattFilter();

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

    // LED pásek – nastav výchozí náladu
    leds.begin();
    const MoodColor& mc = MOOD_COLORS[(int)g_mood];
    leds.setAll(mc.r, mc.g, mc.b);

    // Akcelerometr + motor
    if (!accel.begin()) {
        Serial.println("WARN: ADXL345 nenalezen!");
    }
    motor.begin();

    // Web server – AP se spustí zde
    webui.begin(ssid, [](const DrawMsg& msg) {
        display.handleDraw(msg);
    });

    // Info o připojení – zůstane na displeji dokud uživatel nepřepne mód
    display.showConnectInfo(ssid, WIFI_PASS, WiFi.softAPIP().toString().c_str());

    Serial.println("Setup hotov.");
}

// ══════════════════════════════════════════════════════════════

// ── SOS mód ───────────────────────────────────────────────────
// Info screen je na displeji po bootu – nechceme ho přepsat při prvním loop()
// Displej se smaže teprve při přechodu z jiného módu do DRAWING
AppMode _lastMode     = AppMode::DRAWING;
Mood    _lastMood     = Mood::SMUTNY;

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
        display.clear(ST77XX_BLACK);
        leds.clearAll();
    }

    accel.update();
    TiltDir  dir    = accel.getTiltDir();
    int      duty   = accel.getMotorDuty();
    uint32_t period = accel.getBlinkPeriod();

    // Horizont čára: rovně = vodorovně, doleva = čára se kloní doprava
    // getY() < 0 = tilt doleva → half_dy kladné → pravý konec výš
    int half_dy = constrain((int)(-accel.getY() * 15.0f), -(DISP_H/2 - 20), (DISP_H/2 - 20));
    display.drawHorizonLine(half_dy);

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
