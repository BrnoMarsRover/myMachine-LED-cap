/* =============================================================
 * Project:   myMachine LED Cap
 * File:      myMachine_LED_cap.ino
 * Author:    Martin Kriz
 * Company:   Brno Mars Rover
 * Platform:  MH-ET LIVE ESP32 MiniKIT (ESP32-WROOM-32)
 * ============================================================= */

#include "include/pins.h"
#include "config.h"
#include "modes.h"
#include "DisplayTFT.h"
#include "WebUI.h"
#include "LedStrip.h"
#include "Accel.h"
#include "include/Motor.h"
#include "images/kid_67.h"
#include "images/police_img.h"
#include "images/car_img.h"

// ── Globální stav ─────────────────────────────────────────────
AppMode g_mode = AppMode::DRAWING;

DisplayTFT display;
WebUI      webui;
LedStrip   leds;
Accel      accel;
Motor      motor;

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
}

// ══════════════════════════════════════════════════════════════
void setup() {
    Serial.begin(115200);

    initPins();

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

    // LED pásek + akcelerometr + motor
    leds.begin();

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

// ── Police mod ────────────────────────────────────────────────
AppMode _lastMode       = AppMode::DRAWING;
bool    _policeBlue     = true;
uint32_t _policeTimer   = 0;

void handlePolice() {
    if (_lastMode != AppMode::POLICE) {
        display.showImage(police_img, POLICE_IMG_W, POLICE_IMG_H);
        _policeTimer = millis();
        _policeBlue  = true;
        leds.setAll(0, 0, 255);
    }
    if (millis() - _policeTimer >= 500) {
        _policeTimer = millis();
        _policeBlue  = !_policeBlue;
        if (_policeBlue) leds.setAll(0, 0, 255);
        else             leds.setAll(255, 0, 0);
    }
}

// ── Accel mod ─────────────────────────────────────────────────
uint32_t _accelBlinkTimer = 0;
bool     _accelBlinkOn    = false;
TiltDir  _lastTiltDir     = TiltDir::NONE;

void handleAccel() {
    if (_lastMode != AppMode::ACCEL) {
        display.showImage(car_img, CAR_IMG_W, CAR_IMG_H);
        leds.clearAll();
    }

    accel.update();
    TiltDir dir     = accel.getTiltDir();
    int     duty    = accel.getMotorDuty();
    uint32_t period = accel.getBlinkPeriod();

    // Motor – rychlost podle náklonu, směr podle levá/pravá
    motor.stop();
    if (dir == TiltDir::RIGHT) motor.forward(duty);
    else if (dir == TiltDir::LEFT) motor.reverse(duty);
    motor.dutyUpdate();

    // LED blikání
    if (millis() - _accelBlinkTimer >= period / 2) {
        _accelBlinkTimer = millis();
        _accelBlinkOn    = !_accelBlinkOn;

        leds.clearAll();
        if (_accelBlinkOn && dir != TiltDir::NONE) {
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

    switch (g_mode) {
        case AppMode::DRAWING:
            if (_lastMode != AppMode::DRAWING) {
                leds.clearAll();
                motor.stop();
                display.clear(ST77XX_WHITE);
            }
            break;

        case AppMode::POLICE:
            handlePolice();
            break;

        case AppMode::ACCEL:
            handleAccel();
            break;
    }

    _lastMode = g_mode;
}
