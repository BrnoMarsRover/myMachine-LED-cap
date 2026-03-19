/* =============================================================
 * Project:   myMachine LED Cap
 * File:      Accelerometer.h
 * Author:    Martin Kriz
 * Company:   Brno Mars Rover
 * -------------------------------------------------------------
 * Description:
 *   Accelerometer class – ADXL345BCCZ over I2C (addr 0x53).
 *   Pins: SDA=21, SCL=22, INT=17
 *   Library: Adafruit ADXL345
 * ============================================================= */

#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

constexpr int A_SDA_PIN = 21;
constexpr int A_SCL_PIN = 22;
constexpr int A_INT_PIN = 17;

class Accelerometer {
public:
    Accelerometer() : _adxl(12345) {}

    bool begin() {
        Wire.begin(A_SDA_PIN, A_SCL_PIN);
        if (!_adxl.begin(0x53)) {
            return false;
        }
        _adxl.setRange(ADXL345_RANGE_4_G);
        pinMode(A_INT_PIN, INPUT);
        return true;
    }

    void update() {
        sensors_event_t event;
        _adxl.getEvent(&event);
        _x = event.acceleration.x;
        _y = event.acceleration.y;
        _z = event.acceleration.z;
    }

    float getX() const { return _x; }
    float getY() const { return _y; }
    float getZ() const { return _z; }

    void printInfo() {
        sensor_t sensor;
        _adxl.getSensor(&sensor);
        Serial.printf("Sensor: %s | Max: %.1f m/s2 | Res: %.4f m/s2\n",
            sensor.name, sensor.max_value, sensor.resolution);
    }

private:
    Adafruit_ADXL345_Unified _adxl;
    float _x = 0, _y = 0, _z = 0;
};
