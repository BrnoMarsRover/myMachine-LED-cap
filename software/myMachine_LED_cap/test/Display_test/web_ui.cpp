#include "web_ui.h"
#include "web_pages.h"

void WebUI::begin(DrawMsgCallback cb) {
  onDraw = cb;

  // WiFi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASS);

  Serial.println();
  Serial.print("AP SSID: "); Serial.println(WIFI_SSID);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  // WS handler
  ws.onEvent([this](AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType t, void *a, uint8_t *d, size_t l){
    this->onWsEvent(s, c, t, a, d, l);
  });
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
  Serial.println("Web server spusten.");
}

void WebUI::loop() {
  ws.cleanupClients();
}

void WebUI::onWsEvent(AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len)
{
  if (type != WS_EVT_DATA) return;

  AwsFrameInfo* info = (AwsFrameInfo*)arg;
  if (!(info->final && info->opcode == WS_TEXT && info->index == 0 && info->len == len)) return;

  String msg;
  msg.reserve(len);
  for (size_t i = 0; i < len; i++) msg += (char)data[i];

  // očekávám: clientId|payload
  int pipe = msg.indexOf('|');
  if (pipe <= 0) return;

  String payload = msg.substring(pipe + 1);

  // broadcast všem (multi-device)
  ws.textAll(msg);

  // parse payload
  DrawMsg dm;
  if (!parseDrawPayload(payload, dm)) return;

  if (onDraw) onDraw(dm);
}
