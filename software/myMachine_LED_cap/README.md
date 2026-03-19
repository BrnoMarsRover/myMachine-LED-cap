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

## Claude todo
hlavni kod -

init - vsechny piny
nacteni id pinu - podle toho se pak bude hlasit dane esp pod wifi, wifi: ksiltovka_x kde x bude prave to nactene id

na 5s zobrazit fotku s nazvem kid_67 kterou ti tam pak nahraju, jen na to priprav misto 

potom uz to bude zavisle na modu kterej si vybere uzivatel pres tu wifi, ten web mame pripravej asi bude potreba tam pridat nejake tlacitko pro prepinani modu 

mody - 

klasicke kresleni - na displeji se zobrazi co si uzivatel nakresli pres ten canvas

policie - na displeji se zobrazi fotka kterou ti zas dodam jen na ni priprav soubor, led pasek bude blikat modro-cervene

accel - podle dat z akcelerometru se bude rozsvicet segment led pasku (left: 29 az 83 front:112 az 170 a right: 1 az 28 a 84 az 111) podle toho kam bude gravitace tak to bude oranzove blikat jako blink u auta, jeste podle toho jako moc to bude nakrivo tam cim vic nakrivo tim rychleji se bude tocit motor, a na displeji bude fotka car na kterou zase priprav misto 


