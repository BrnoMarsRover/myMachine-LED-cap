# myMachine LED Cap – Software

ESP32 firmware pro LED čepici. Platform: **MH-ET LIVE ESP32 MiniKIT (ESP32-WROOM-32)**.

## Požadované knihovny

Nainstaluj přes **Arduino IDE → Tools → Manage Libraries**:

| Knihovna | Autor | Použití |
|---|---|---|
| `Adafruit ADXL345` | Adafruit | Akcelerometr ADXL345BCCZ |
| `Adafruit Unified Sensor` | Adafruit | Závislost Adafruit senzorových knihoven |
| `Adafruit ST7796S` | Adafruit | Displej ST7796S |
| `Adafruit GFX Library` | Adafruit | Grafická vrstva pro displej |
| `Adafruit NeoPixel` | Adafruit | LED pásek WS2812B |

## Struktura projektu

```
myMachine_LED_cap/
├── myMachine_LED_cap.ino   # Hlavní sketch
├── pinout.md               # Popis pinů ESP32
├── include/
│   ├── pins.h              # Definice pinů
│   └── Motor.h             # Třída pro motor driver
└── test/
    ├── Motor_test/         # Test DC motoru
    ├── Accelerometer_test/ # Test ADXL345
    ├── Led_test/           # Test LED pásku WS2812B
    └── Display_test/       # Test displeje ST7796S
```
