# myMachine LED Cap – Software

ESP32 firmware pro LED čepici projektu Brno Mars Rover.
Hardware: **MH-ET LIVE ESP32 MiniKIT (ESP32-WROOM-32)**

---

## Jak flashovat

### 1. Arduino IDE

Stáhni a nainstaluj [Arduino IDE 2.x](https://www.arduino.cc/en/software).

### 2. Přidej podporu pro ESP32

**File → Preferences → Additional boards manager URLs:**
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
Pak **Tools → Board → Boards Manager** → nainstaluj `esp32 by Espressif Systems`.

### 3. Nainstaluj knihovny

**Tools → Manage Libraries** – nainstaluj tyto:

| Knihovna | Autor | Poznámka |
|---|---|---|
| `Adafruit GFX Library` | Adafruit | Grafická vrstva displeje |
| `Adafruit ST7796S` | Adafruit | Driver displeje ST7796S |
| `Adafruit BusIO` | Adafruit | Závislost SPI/I2C knihoven |
| `Adafruit NeoPixel` | Adafruit | LED pásek WS2812B |
| `Adafruit ADXL345` | Adafruit | Akcelerometr ADXL345BCCZ |
| `Adafruit Unified Sensor` | Adafruit | Závislost senzorových knihoven |
| `AsyncTCP` | ESP32-focusight | WebSocket – asynchronní TCP |
| `ESPAsyncWebServer` | ESP32-focusight | WebSocket – async HTTP/WS server |


### 4. Nahrání

Připoj ESP32 přes USB, klikni **Upload** (→). Pokud ESP32 nereaguje, drž tlačítko **BOOT** na desce během uploadu.

---

## Jak se připojit (uživatel)

Po zapnutí čepice:

1. **Boot screen** – zobrazí se obrázek na ~5 sekund
2. **Info screen** – na displeji se zobrazí:
   - Název WiFi sítě: `ksiltovka_X` (X = číslo čepice dle HW jumperů)
   - Heslo: `12345678`
   - Adresa: `http://192.168.4.1`
3. Připoj se na WiFi síť `ksiltovka_X` a v prohlížeči otevři `http://192.168.4.1`
4. Zobrazí se webové rozhraní – odtud lze přepínat módy a kreslit

---

## Módy

### Kreslení
Výchozí mód po startu. Na displeji je prázdné bílé plátno, na které lze kreslit prstem nebo myší přes webové rozhraní. V záložce **KRESLENÍ** lze šipkami ◀ ▶ vybrat náladu, která nastaví barvu LED pásku:

| Nálada | Barva LED |
|---|---|
| Smutný | Modrá |
| Veselý | Žlutá |
| Naštvaný | Červená |
| Překvapený | Oranžová |
| Vyděšený | Zelená |
| Nadšený | Fialová |

### SOS
LED pásek bliká střídavě modro-červeně (vzor záchranky). Na displeji se zobrazí uložený obrázek.
Aktivace: záložka **SOS** ve webovém rozhraní.

### Blinkr
Čepice funguje jako blinkr auta – reaguje na náklon pomocí akcelerometru:
- **Náklon doleva** → levá část LED pásku bliká oranžově + motor se otočí
- **Náklon doprava** → pravá část LED pásku bliká oranžově + motor se otočí opačně
- **Rovně** → žádné blikání
- Na displeji je žlutá horizont čára, která se naklání opačně než čepice (simuluje rovný horizont)
- Čím větší náklon, tím rychlejší blikání a větší výkon motoru

Aktivace: záložka **BLINKR** ve webovém rozhraní.

---

## Baterie

Čepice měří napětí baterie přes ADC. Při poklesu pod `BATT_RAW_MIN` (viz `config.h`):
1. Zobrazí se nápis **"Vybita baterie"**
2. LED pásek zhasne
3. ESP32 přejde do deep sleep na 60 sekund, poté se znovu zkontroluje napětí

Kalibrační hodnoty ADC jsou v `include/config.h`:
```cpp
#define BATT_RAW_MAX  3900   // ADC raw @ ~4.2 V (100 %)
#define BATT_RAW_MIN  3100   // ADC raw @ ~3.3 V (  0 %)
```
Tyto hodnoty je potřeba zkalibrovat podle skutečného měření na konkrétní desce.

---

## Struktura projektu

```
myMachine_LED_cap/
├── myMachine_LED_cap.ino   # Hlavní sketch – setup, loop, logika módů
├── DisplayTFT.cpp          # Displej – implementace
├── WebUI.cpp               # Web server – implementace
├── web_pages.cpp           # HTML stránka (PROGMEM)
├── partitions.csv          # Custom partition table (3MB app)
├── include/
│   ├── pins.h              # Definice pinů ESP32
│   ├── config.h            # WiFi heslo, rozměry, kalibrační konstanty
│   ├── modes.h             # Enum AppMode + Mood
│   ├── DisplayTFT.h        # Třída displeje ST7796S
│   ├── WebUI.h             # Třída AsyncWebServer + WebSocket
│   ├── LedStrip.h          # NeoPixel – segmenty RIGHT/LEFT/FRONT
│   ├── Accel.h             # ADXL345 – tilt detekce, motor duty, blink period
│   ├── Motor.h             # Motor driver (LEDC PWM)
│   ├── ws_protocol.h       # Protokol WebSocket zpráv kreslení
│   └── web_pages.h         # Deklarace index_html
└── images/
    ├── kid_67.h            # Boot obrázek
    ├── police_img.h        # Obrázek pro SOS mód
    ├── car_img.h           # Obrázek pro Blinkr mód (placeholder)
    └── convert_policie.py  # Skript: LVGL .c → Arduino .h (RGB565)
```

---

## TODO

- [ ] Dodat `skibidi.c` → spustit `convert_policie.py` → nahradit `images/car_img.h`
- [ ] Zkalibrovat `BATT_RAW_MAX` a `BATT_RAW_MIN` podle skutečného ADC měření
- [ ] Ověřit směr náklonu Blinkr módu fyzicky – případně nastavit `TILT_INVERT_Y = 1` v `config.h`
