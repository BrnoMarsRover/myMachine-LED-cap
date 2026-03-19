/* =============================================================
 * Project:   myMachine LED Cap
 * File:      pins.h
 * Author:    Martin Kriz
 * Company:   Ullmanna s.r.o.
 * -------------------------------------------------------------
 * Description:
 *   Pin definitions for MH-ET LIVE ESP32 MiniKIT (ESP32-WROOM-32).
 *   Based on pinout.md.
 *
 * Legend:
 *   M_   – DC motor driver (DRV8873H or similar)
 *   D_   – display (SPI)
 *   LED_ – LED strip (WS281x / one-wire data)
 *   A_   – accelerometer (I2C)
 *   B_   – battery
 *   ID_  – hardware identification
 * ============================================================= */

#pragma once

// ── Motor driver ──────────────────────────────────────────────
#define PIN_M_IPROP   36   // ADC1_CH0  – motor current (analog in, ADC1 – Wi-Fi safe)
#define PIN_M_SLEEP   32   // GPIO32    – motor sleep (LOW = sleep)
#define PIN_M_FAULT   25   // GPIO25    – motor fault flag (active LOW)
#define PIN_M_DIR     2    // GPIO2     – motor direction
#define PIN_M_PWM     0    // GPIO0     – motor PWM  ⚠ LOW při bootu = flash mode

// ── Display (SPI) ─────────────────────────────────────────────
#define PIN_D_RESET   33   // GPIO33    – display reset (active LOW)
#define PIN_D_DC      14   // GPIO14    – display data/command (DC/RS)
#define PIN_D_CS      5    // GPIO5     – display chip select (SPI CS)
#define PIN_D_SCK     18   // GPIO18    – display SPI clock (VSPI SCK)
#define PIN_D_MOSI    19   // GPIO19    – display SPI MOSI (VSPI MISO pin, remapped)

// ── LED strip ─────────────────────────────────────────────────
#define PIN_LED_DATA  4    // GPIO4     – LED strip data (WS281x)

// ── Accelerometer (I2C) ───────────────────────────────────────
#define PIN_A_SDA     21   // GPIO21    – I2C SDA
#define PIN_A_SCL     22   // GPIO22    – I2C SCL
#define PIN_A_INT     17   // GPIO17    – accelerometer interrupt

// ── Battery ───────────────────────────────────────────────────
#define PIN_B_STATUS  34   // GPIO34    – battery status (analog in, ADC1 – Wi-Fi safe, input only)

// ── Hardware identification ───────────────────────────────────
#define PIN_ID_0      26   // GPIO26    – HW ID bit 0
#define PIN_ID_1      27   // GPIO27    – HW ID bit 1
