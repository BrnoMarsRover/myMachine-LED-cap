#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#include "config.h"

constexpr int ACCEL_SDA = 21;
constexpr int ACCEL_SCL = 22;
constexpr int ACCEL_INT = 17;

enum class TiltDir { NONE, LEFT, RIGHT, FRONT };

class Accel {
public:
    Accel() : _adxl(12345) {}

    bool begin() {
        Wire.begin(ACCEL_SDA, ACCEL_SCL);
        if (!_adxl.begin(0x53)) return false;
        _adxl.setRange(ADXL345_RANGE_4_G);
        pinMode(ACCEL_INT, INPUT);
        return true;
    }

    void update() {
        sensors_event_t event;
        _adxl.getEvent(&event);
        _ax = event.acceleration.x;
        _ay = event.acceleration.y;
        _az = event.acceleration.z;
    }

    float getX() const { return _ax; }
    float getY() const { return _ay; }
    float getZ() const { return _az; }

    // Úhel náklonu od rovné polohy v stupních (0 = rovně)
    // Naměřeno: v klidu X≈10 (gravitace), Y≈0, Z≈-2
    // Boční tilt (L/R) odpovídá odchylce osy Y od nuly
    float getTiltAngle() const {
        return degrees(atan2(abs(_ay), abs(_ax)));
    }

    // Dominantní směr náklonu podle naměřené orientace senzoru:
    //   doleva  = Y záporná,  doprava = Y kladná
    //   dopredu = Z kladná (z klidové hodnoty -2 se zvýší na +4)
    // Kalibrace přes TILT_INVERT_Y v config.h
    TiltDir getTiltDir() const {
        float ay = TILT_INVERT_Y ? -_ay : _ay;
        if (abs(ay) > TILT_THRESHOLD) {
            return (ay > 0) ? TiltDir::RIGHT : TiltDir::LEFT;
        }
        if (_az > TILT_THRESHOLD) {
            return TiltDir::FRONT;
        }
        return TiltDir::NONE;
    }

    // Výkon motoru 0–100 podle náklonu
    int getMotorDuty(float minAngle = 5.0f, float maxAngle = 45.0f) const {
        float angle = getTiltAngle();
        if (angle < minAngle) return 0;
        if (angle > maxAngle) return 100;
        return (int)((angle - minAngle) / (maxAngle - minAngle) * 100.0f);
    }

    // Perioda blikání v ms podle náklonu (více nakloněno = rychleji)
    uint32_t getBlinkPeriod(uint32_t fast = 150, uint32_t slow = 700) const {
        float angle = getTiltAngle();
        float t = constrain(angle / 45.0f, 0.0f, 1.0f);
        return (uint32_t)(slow - t * (slow - fast));
    }

private:
    Adafruit_ADXL345_Unified _adxl;
    float _ax = 0, _ay = 0, _az = 9.81f;
};
