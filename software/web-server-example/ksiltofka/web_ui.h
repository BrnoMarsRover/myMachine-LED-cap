#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "config.h"
#include "ws_protocol.h"

typedef void (*DrawMsgCallback)(const DrawMsg& msg);

class WebUI {
public:
  void begin(DrawMsgCallback cb);
  void loop();

private:
  AsyncWebServer server = AsyncWebServer(80);
  AsyncWebSocket ws = AsyncWebSocket("/ws");
  DrawMsgCallback onDraw = nullptr;

  void onWsEvent(AsyncWebSocket *server,
                 AsyncWebSocketClient *client,
                 AwsEventType type,
                 void *arg,
                 uint8_t *data,
                 size_t len);
};
