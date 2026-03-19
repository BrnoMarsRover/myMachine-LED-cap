#include "Display.h"

Display display;

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Starting ST7796S display test...");

    display.begin();
    Serial.println("Display initialized.");

    // === Color fill test ===
    Serial.println("RED");
    display.clear(ST77XX_RED);
    delay(800);

    Serial.println("GREEN");
    display.clear(ST77XX_GREEN);
    delay(800);

    Serial.println("BLUE");
    display.clear(ST77XX_BLUE);
    delay(800);

    // === Text test ===
    display.clear();
    display.printText("myMachine LED Cap", 10, 10, 2, ST77XX_WHITE);
    display.printText("ST7796S OK", 10, 40, 2, ST77XX_GREEN);
    Serial.println("Text drawn.");

    // === Rectangle test ===
    display.fillRect(10,  80, 80, 80, ST77XX_RED);
    display.fillRect(100, 80, 80, 80, ST77XX_GREEN);
    display.fillRect(190, 80, 80, 80, ST77XX_BLUE);
    Serial.println("Rectangles drawn.");
}

void loop() {
    // static display – no loop action needed
}
