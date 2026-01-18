#include <Arduino.h>

#include "display_tft.h"
#include "web_ui.h"

DisplayTFT display;
WebUI webui;

void onDrawMsg(const DrawMsg& msg) {
  display.handleDraw(msg);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  display.begin();
  display.showBootScreen();

  webui.begin(onDrawMsg);
}

void loop() {
  webui.loop();
}
