/* =============================================================
 * Project:   myMachine LED Cap
 * File:      Motor.h
 * Author:    Martin Kriz
 * Company:   Brno Mars Rover
 * -------------------------------------------------------------
 * Description:
 *   DC motor driver class (DRV8873H), ESP32 / LEDC PWM.
 *   Pins: SLEEP=32, DIR=2, PWM=0, FAULT=25
 * ============================================================= */

#pragma once
#include <Arduino.h>

constexpr int  MOTOR_NSLEEP   = 32;
constexpr int  MOTOR_DISABLE  = -1;   // nepouzito na tomto HW
constexpr int  MOTOR_PH       = 2;
constexpr int  MOTOR_EN       = 0;
constexpr int  MOTOR_NFAULT   = 25;

constexpr int  MOTOR_PWM_FREQ = 25000;
constexpr int  MOTOR_PWM_BITS = 8;

class Motor {
public:
    Motor()
        : _nSleep(MOTOR_NSLEEP), _disable(MOTOR_DISABLE),
          _ph(MOTOR_PH), _en(MOTOR_EN), _nFault(MOTOR_NFAULT) {}

    Motor(int nSleep, int disable, int ph, int en, int nFault)
        : _nSleep(nSleep), _disable(disable), _ph(ph), _en(en), _nFault(nFault) {}

    void begin() {
        pinMode(_nSleep, OUTPUT);
        if (_disable >= 0) pinMode(_disable, OUTPUT);
        pinMode(_ph, OUTPUT);
        ledcAttach(_en, MOTOR_PWM_FREQ, MOTOR_PWM_BITS);
        pinMode(_nFault, INPUT);
        wake();
        enable();
        stop();
    }

    void dutyUpdate() {
        if (_currentDuty == _targetDuty) return;
        if (millis() - _lastUpdate >= _rampDelay) {
            if (_currentDuty < _targetDuty) _currentDuty++;
            else                             _currentDuty--;

            digitalWrite(_ph, _currentDuty >= 0 ? HIGH : LOW);
            ledcWrite(_en, map(abs(_currentDuty), 0, 100, 0, 255));
            _lastUpdate = millis();
        }
    }

    void wake()    { digitalWrite(_nSleep, HIGH); }
    void sleep()   { digitalWrite(_nSleep, LOW); }
    void enable()  { if (_disable >= 0) digitalWrite(_disable, LOW); }
    void disable() { if (_disable >= 0) digitalWrite(_disable, HIGH); }
    void stop()    { _targetDuty = 0; }

    void forward(int16_t duty) { _targetDuty =  constrain(duty, 0, 100); }
    void reverse(int16_t duty) { _targetDuty = -constrain(duty, 0, 100); }

    void resetFault() {
        digitalWrite(_nSleep, LOW);
        delayMicroseconds(12);
        digitalWrite(_nSleep, HIGH);
    }

    bool isFault() { return digitalRead(_nFault) == LOW; }

private:
    int _nSleep, _disable, _ph, _en, _nFault;
    int16_t  _currentDuty = 0;
    int16_t  _targetDuty  = 0;
    uint32_t _lastUpdate  = 0;
    uint16_t _rampDelay   = 10;
};
