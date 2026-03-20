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

    // Úhel náklonu v stupních (0 = rovně, 90 = na boku)
    float getTiltAngle() const {
        float horiz = sqrt(_ax * _ax + _ay * _ay);
        return degrees(atan2(horiz, abs(_az)));
    }

    // Dominantní směr náklonu – kalibrace přes TILT_INVERT_X/Y v config.h
    TiltDir getTiltDir() const {
        float absX = abs(_ax);
        float absY = abs(_ay);
        if (absX < TILT_THRESHOLD && absY < TILT_THRESHOLD) return TiltDir::NONE;
        if (absX >= absY) {
            bool goRight = (_ax > 0);
            if (TILT_INVERT_X) goRight = !goRight;
            return goRight ? TiltDir::RIGHT : TiltDir::LEFT;
        }
        bool goFront = (_ay > 0);
        if (TILT_INVERT_Y) goFront = !goFront;
        return goFront ? TiltDir::FRONT : TiltDir::NONE;
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
