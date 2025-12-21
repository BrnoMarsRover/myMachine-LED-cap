#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "ESP32-AP";
const char* password = "12345678";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// HTML s canvasem, výběrem barev a WebSocketem
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Canvas</title>
<style>
  body {
    margin: 0;
    background: #222;
    color: #fff;
    font-family: sans-serif;
    text-align: center;
  }
  #wrapper {
    display: flex;
    justify-content: center;
    align-items: flex-start;
    margin-top: 10px;
  }
  #canvas {
    touch-action: none;
    border: 1px solid #555;
    background: #000;
  }
  #toolbar {
    display: flex;
    flex-direction: column;
    margin-left: 8px;
    gap: 6px;
  }
  .colorBtn {
    width: 36px;
    height: 36px;
    border-radius: 50%;
    border: 2px solid #444;
    padding: 0;
    cursor: pointer;
  }
  .colorBtn.active {
    border-color: #fff;
    box-shadow: 0 0 5px #fff;
  }
  #clearBtn {
    margin-top: 10px;
    padding: 6px 8px;
    cursor: pointer;
  }
</style>
</head>
<body>
<h2>ESP32 –canvas</h2>

<div id="wrapper">
  <canvas id="canvas" width="240" height="320"></canvas>
  <div id="toolbar">
    <button class="colorBtn active" data-color="#00ff00" style="background:#00ff00;"></button>
    <button class="colorBtn" data-color="#ff0000" style="background:#ff0000;"></button>
    <button class="colorBtn" data-color="#0000ff" style="background:#0000ff;"></button>
    <button class="colorBtn" data-color="#ffff00" style="background:#ffff00;"></button>
    <button class="colorBtn" data-color="#ffffff" style="background:#ffffff;"></button>
    <button class="colorBtn" data-color="#ff00ff" style="background:#ff00ff;"></button>
    <button class="colorBtn" data-color="#ff8800" style="background:#ff8800;"></button>
    <button id="clearBtn">Vymazat</button>
  </div>
</div>

<p>Kresli prstem nebo myší.</p>

<script>
  const canvas = document.getElementById('canvas');
  const ctx = canvas.getContext('2d');

  let drawing = false;
  let lastX = 0;
  let lastY = 0;
  let currentColor = '#00ff00'; // výchozí barva

  // nastavení pera
  ctx.strokeStyle = currentColor;
  ctx.lineWidth = 2;
  ctx.lineJoin = 'round';
  ctx.lineCap = 'round';

  // Výběr barvy
  const colorButtons = document.querySelectorAll('.colorBtn');
  colorButtons.forEach(btn => {
    btn.addEventListener('click', () => {
      currentColor = btn.dataset.color;
      ctx.strokeStyle = currentColor;

      // vizuálně označit aktivní barvu
      colorButtons.forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
    });
  });

  // WebSocket připojení
  let socket;

  function connectWS() {
    socket = new WebSocket('ws://' + window.location.host + '/ws');

    socket.onopen = () => {
      console.log('WebSocket připojen');
    };

    socket.onclose = () => {
      console.log('WebSocket odpojen, zkusím znovu...');
      setTimeout(connectWS, 1000);
    };

    socket.onerror = (err) => {
      console.log('WebSocket error', err);
    };
  }

  connectWS();

  function sendPoint(type, x, y) {
    if (socket && socket.readyState === WebSocket.OPEN) {
      // typ:x,y:barva  (např. "move:123,45:#ff0000")
      socket.send(
        type + ':' +
        Math.round(x) + ',' + Math.round(y) + ':' +
        currentColor
      );
    }
  }

  function getPos(e) {
    const rect = canvas.getBoundingClientRect();
    let x, y;

    if (e.touches && e.touches.length > 0) {
      x = e.touches[0].clientX - rect.left;
      y = e.touches[0].clientY - rect.top;
    } else {
      x = e.clientX - rect.left;
      y = e.clientY - rect.top;
    }

    return { x, y };
  }

  function startDraw(e) {
    e.preventDefault();
    const pos = getPos(e);
    drawing = true;
    lastX = pos.x;
    lastY = pos.y;
    sendPoint('start', pos.x, pos.y);
  }

  function moveDraw(e) {
    if (!drawing) return;
    e.preventDefault();
    const pos = getPos(e);

    // kreslení na canvas (lokálně)
    ctx.beginPath();
    ctx.moveTo(lastX, lastY);
    ctx.lineTo(pos.x, pos.y);
    ctx.stroke();

    // poslání na ESP
    sendPoint('move', pos.x, pos.y);

    lastX = pos.x;
    lastY = pos.y;
  }

  function endDraw(e) {
    if (!drawing) return;
    e.preventDefault();
    drawing = false;
    sendPoint('end', 0, 0);
  }

  // myš
  canvas.addEventListener('mousedown', startDraw);
  canvas.addEventListener('mousemove', moveDraw);
  canvas.addEventListener('mouseup', endDraw);
  canvas.addEventListener('mouseleave', endDraw);

  // dotyk
  canvas.addEventListener('touchstart', startDraw);
  canvas.addEventListener('touchmove', moveDraw);
  canvas.addEventListener('touchend', endDraw);
  canvas.addEventListener('touchcancel', endDraw);

  // clear tlačítko – jen lokálně
  document.getElementById('clearBtn').addEventListener('click', () => {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
  });
</script>
</body>
</html>
)rawliteral";

// WebSocket handler – teď navíc parsuje barvu
void onWsEvent(AsyncWebSocket *server,
               AsyncWebSocketClient *client,
               AwsEventType type,
               void *arg,
               uint8_t *data,
               size_t len)
{
  if (type == WS_EVT_DATA) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;

    if (info->final && info->opcode == WS_TEXT && info->index == 0 && info->len == len) {
      String msg;
      msg.reserve(len);
      for (size_t i = 0; i < len; i++) {
        msg += (char)data[i];
      }

      // očekáváme: "typ:x,y:barva"  (např. "move:123,45:#ff0000")
      int firstColon   = msg.indexOf(':');
      int secondColon  = msg.indexOf(':', firstColon + 1);
      int commaIndex   = msg.indexOf(',', firstColon + 1);

      if (firstColon > 0 && secondColon > firstColon && commaIndex > firstColon && commaIndex < secondColon) {
        String typeStr  = msg.substring(0, firstColon);
        int x           = msg.substring(firstColon + 1, commaIndex).toInt();
        int y           = msg.substring(commaIndex + 1, secondColon).toInt();
        String colorStr = msg.substring(secondColon + 1);  // např. "#ff0000"

        if (typeStr == "start") {
          Serial.printf("START x=%d y=%d color=%s\n", x, y, colorStr.c_str());
        } else if (typeStr == "move") {
          Serial.printf("MOVE  x=%d y=%d color=%s\n", x, y, colorStr.c_str());
        } else if (typeStr == "end") {
          Serial.printf("END   color=%s\n", colorStr.c_str());
        }
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Startuji WiFi AP...");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP bezi. SSID: ");
  Serial.println(ssid);
  Serial.print("IP adresa: ");
  Serial.println(ip);

  // WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // HTTP root -> stránka s canvasem
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("▶ HTTP GET /");
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
  Serial.println("Web server spusten.");
}

void loop() {
  ws.cleanupClients();
}
