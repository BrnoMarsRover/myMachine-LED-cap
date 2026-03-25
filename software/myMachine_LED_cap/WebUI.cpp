#include "include/WebUI.h"
#include "include/web_pages.h"

void WebUI::begin(const char* ssid, DrawMsgCallback cb) {
    onDraw = cb;

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, WIFI_PASS);
    Serial.printf("AP SSID: %s  IP: %s\n", ssid, WiFi.softAPIP().toString().c_str());

    // WebSocket
    ws.onEvent([this](AsyncWebSocket* s, AsyncWebSocketClient* c,
                      AwsEventType t, void* a, uint8_t* d, size_t l) {
        this->onWsEvent(s, c, t, a, d, l);
    });
    server.addHandler(&ws);

    // Hlavní stránka
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send_P(200, "text/html", index_html);
    });

    // Přepínání módů
    server.on("/mode", HTTP_POST, [](AsyncWebServerRequest* req) {
        if (req->hasParam("mode", true)) {
            String m = req->getParam("mode", true)->value();
            if      (m == "drawing") g_mode = AppMode::DRAWING;
            else if (m == "sos")     g_mode = AppMode::SOS;
            else if (m == "blinkr")  g_mode = AppMode::BLINKR;
            Serial.printf("Mode: %s\n", m.c_str());
        }
        req->send(200, "text/plain", "OK");
    });

    // Stav baterie
    server.on("/battery", HTTP_GET, [](AsyncWebServerRequest* req) {
        int pct = (int)((g_battFiltered - BATT_RAW_MIN) / (float)(BATT_RAW_MAX - BATT_RAW_MIN) * 100.0f);
        if (pct < 0)   pct = 0;
        if (pct > 100) pct = 100;
        req->send(200, "text/plain", String(pct));
    });

    // Přepínání nálad (DRAWING mód)
    server.on("/mood", HTTP_POST, [](AsyncWebServerRequest* req) {
        if (req->hasParam("mood", true)) {
            int m = req->getParam("mood", true)->value().toInt();
            if (m >= 0 && m < (int)Mood::MOOD_COUNT) {
                g_mood = (Mood)m;
                Serial.printf("Mood: %d\n", m);
            }
        }
        req->send(200, "text/plain", "OK");
    });

    server.begin();
    Serial.println("Web server spusten.");
}

void WebUI::loop() {
    ws.cleanupClients();
}

void WebUI::onWsEvent(AsyncWebSocket* server,
                      AsyncWebSocketClient* client,
                      AwsEventType type,
                      void* arg,
                      uint8_t* data,
                      size_t len)
{
    if (type != WS_EVT_DATA) return;

    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (!(info->final && info->opcode == WS_TEXT && info->index == 0 && info->len == len)) return;

    String msg;
    msg.reserve(len);
    for (size_t i = 0; i < len; i++) msg += (char)data[i];

    int pipe = msg.indexOf('|');
    if (pipe <= 0) return;

    String payload = msg.substring(pipe + 1);
    ws.textAll(msg);

    DrawMsg dm;
    if (!parseDrawPayload(payload, dm)) return;
    if (onDraw && g_mode == AppMode::DRAWING) onDraw(dm);
}
