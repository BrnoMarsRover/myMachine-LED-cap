# myMachine LED Cap – Software

ESP32 firmware pro LED čepici. Platform: **MH-ET LIVE ESP32 MiniKIT (ESP32-WROOM-32)**.

---

## Požadované knihovny

Nainstaluj přes **Arduino IDE → Tools → Manage Libraries**:

| Knihovna | Autor | Použití |
|---|---|---|
| `Adafruit ADXL345` | Adafruit | Akcelerometr ADXL345BCCZ |
| `Adafruit Unified Sensor` | Adafruit | Závislost Adafruit senzorových knihoven |
| `Adafruit ST7796S` | Adafruit | Displej ST7796S |
| `Adafruit GFX Library` | Adafruit | Grafická vrstva pro displej |
| `Adafruit NeoPixel` | Adafruit | LED pásek WS2812B |
| `AsyncTCP` | ESP32-focusight | WebSocket server (ZIP z GitHubu) |
| `ESPAsyncWebServer` | ESP32-focusight | WebSocket server (ZIP z GitHubu) |

---

## Struktura projektu

```
myMachine_LED_cap/
├── myMachine_LED_cap.ino   # Hlavní sketch
├── DisplayTFT.cpp          # Displej – implementace
├── WebUI.cpp               # Web server – implementace
├── web_pages.cpp           # HTML stránka (PROGMEM)
├── pinout.md               # Popis pinů ESP32
├── include/
│   ├── pins.h              # Definice pinů
│   ├── Motor.h             # Motor driver (LEDC PWM)
│   ├── config.h            # WiFi heslo, rozměry displeje/canvasu
│   ├── modes.h             # Enum AppMode + extern g_mode
│   ├── ws_protocol.h       # Draw protokol (WebSocket zprávy)
│   ├── DisplayTFT.h        # Třída pro ST7796S displej
│   ├── WebUI.h             # Třída pro AsyncWebServer + WS
│   ├── LedStrip.h          # NeoPixel LED pásek, segmenty
│   ├── Accel.h             # ADXL345 – tilt, motor duty, blink period
│   └── web_pages.h         # Deklarace index_html
├── images/
│   ├── kid_67.h            # Boot obrázek (placeholder – viz TODO)
│   ├── police_img.h        # SOS obrázek (placeholder – viz TODO)
│   └── car_img.h           # Blinkr obrázek (placeholder – viz TODO)
└── test/
    ├── Motor_test/
    ├── Accelerometer_test/
    ├── Led_test/
    └── Display_test/
```

---

## Módy

| Mód | Název v kódu | Popis |
|---|---|---|
| Kreslení | `DRAWING` | Canvas kreslení přes web + volba nálady (barva LED pásku) |
| SOS | `POLICE` | Displej = foto, LED pásek bliká modro-červeně |
| Blinkr | `ACCEL` | Akcelerometr → blikání segmentů jako blinkr auta + motor |

---

## TODO

### Obrázky (největší rozdělaná věc)
- [ ] Exportovat z LVGL image konvertoru jako RGB565, 480×320
- [ ] `67.c` → `images/kid_67.h` (boot screen)
- [ ] `policie.c` → `images/police_img.h` (SOS mód)
- [ ] `skibidi.c` → `images/car_img.h` (Blinkr mód)
- [ ] Po dodání `.c` souborů spustit konverzní skript

### Blinkr mód – kalibrace os
- [ ] Vyzkoušet fyzicky a podle potřeby přepnout `TILT_INVERT_X` / `TILT_INVERT_Y` v `config.h`
- [ ] Zkalibrovat `BATT_RAW_MAX` a `BATT_RAW_MIN` podle skutečného měření ADC na konkrétní desce

## Hotové funkce

- Záložkový UI (KRESLENÍ / SOS / BLINKR)
- Volba nálady v záložce Kreslení (◀ šipky ▶) → barva LED pásku
- Měření napětí baterie s deep sleep pod `BATT_RAW_MIN`, probuzení po 60 s
- Kalibrace os akcelerometru přes `TILT_INVERT_X/Y` v `config.h`
- Přejmenování módů: POLICE → SOS, ACCEL → BLINKR
