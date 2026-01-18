#include "web_pages.h"

// Kompletní web (iOS landscape fix + modelCanvas + sync + localStorage)
// - PORTRAIT: velký canvas nahoře, toolbar dole
// - LANDSCAPE: canvas co největší vlevo, toolbar vpravo (scrollovatelný, nikdy nezmizí)
// - nevymaže se při otočení / resize
// - nový klient si vyžádá sync a dostane PNG (320x240) od jiného klienta
// - kresba držena v modelCanvas 320x240 => žádné posuny/zmenšení
// - localStorage snapshot jako fallback
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport"
      content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
<title>ESP32 Canvas</title>

<style>
  html, body {
    margin: 0;
    padding: 0;
    height: 100%;
    background:#222;
    color:#fff;
    font-family:sans-serif;
    text-align:center;

    /* důležité: nechceme scroll stránky, ale v landscape chceme scroll toolbaru */
    overflow: hidden;

    /* tvrdě vypnout pinch/zoom */
    touch-action: none;
    -webkit-text-size-adjust: 100%;
  }

  /* body jako flex sloupec: header + pracovní plocha */
  body{
    display:flex;
    flex-direction:column;
    min-height:100vh;
  }

  #header{
    flex: 0 0 auto;
    padding: 6px 8px 0;
  }

  h2 { margin: 6px 0 4px; }
  p  { margin: 6px 0 0; font-size: 14px; color:#ddd; }

  /* wrapper = pracovní plocha (zbytek obrazovky) */
  #wrapper{
    flex: 1 1 auto;
    min-height: 0;            /* iOS: umožní vnitřní scroll */
    display:flex;
    justify-content:center;
    align-items:center;
    gap:10px;
    padding: 8px;
    box-sizing: border-box;
    max-width: 100vw;

    /* DEFAULT (PORTRAIT): canvas nahoře, toolbar dole */
    flex-direction: column;
  }

  /* canvas */
  #canvas{
    background:#fff;
    border:1px solid #555;
    touch-action:none;
    display:block;
  }

  /* toolbar v portrait dole */
  #toolbar{
    display:flex;
    flex-direction: row;
    flex-wrap: wrap;
    gap:6px;
    justify-content:center;
    align-items:center;
    width: 100%;
    max-width: 720px;
  }

  .colorBtn {
    width:36px;
    height:36px;
    border-radius:50%;
    border:2px solid #444;
    padding:0;
    cursor:pointer;
  }
  .colorBtn.active {
    border-color:#fff;
    box-shadow:0 0 5px #fff;
  }

  .sizeRow {
    display:flex;
    gap:6px;
    justify-content:center;
    align-items:center;
    width: 100%;
  }

  .sizeBtn {
    width:36px;
    height:32px;
    border-radius:10px;
    border:2px solid #444;
    background:#333;
    color:#fff;
    cursor:pointer;
  }
  .sizeBtn.active {
    border-color:#fff;
    box-shadow:0 0 5px #fff;
  }

  #sizeValue {
    color:#fff;
    font-weight:bold;
    text-align:center;
    min-width:32px;
    line-height:32px;
  }

  #clearBtn {
    margin-top:6px;
    padding:6px 10px;
    cursor:pointer;
  }

  /* LANDSCAPE: canvas vlevo (flex), toolbar vpravo a scrollovatelný */
  @media (orientation: landscape) {
    #wrapper{
      flex-direction: row;
      align-items: stretch;
      width: 100%;
    }

    #canvas{
      flex: 1 1 auto;
    }

    #toolbar{
      flex: 0 0 170px;
      width: 170px;
      max-width: 170px;

      flex-direction: column;
      flex-wrap: nowrap;
      align-items:center;

      max-height: 100%;
      overflow-y: auto;                 /* ať tlačítka nikdy nezmizí */
      -webkit-overflow-scrolling: touch;
      padding-bottom: 8px;
      box-sizing: border-box;
    }

    /* v landscape ať sizeRow není 100% šířky stránky, ale panelu */
    .sizeRow{ width: auto; }
  }

  /* drobné zhuštění pro malé iPhony */
  @media (max-width: 420px){
    .colorBtn { width:32px; height:32px; }
  }
  /* iOS/Safari: nedovol tlačítkům divné "native" zploštění */
button{
  -webkit-appearance: none;
  appearance: none;
}

/* nikdy nenech prvky v toolbaru smrsknout */
#toolbar, #toolbar *{
  flex-shrink: 0;
}

/* LANDSCAPE: zmenšit header, aby canvas nebyl nízký */
@media (orientation: landscape) {
  #header{
    padding: 2px 8px 0;
  }
  h2{
    margin: 2px 0 2px;
    font-size: 22px; /* menší nadpis */
  }
  #header p{
    display: none;   /* v landscape skryj popisek = víc místa */
  }

  /* méně paddingu ve wrapperu -> víc místa pro canvas */
  #wrapper{
    padding: 4px 6px;
    gap: 8px;
  }

  /* toolbar trochu širší, ať tlačítka nejsou "na krev" */
  #toolbar{
    flex: 0 0 190px;
    width: 190px;
    max-width: 190px;
  }

  /* barvy zpět jako kruhy (a lehce větší) */
  .colorBtn{
    width: 38px;
    height: 38px;
    border-radius: 50%;
  }

  /* tlačítka ať mají stabilní výšku */
  .sizeBtn{
    height: 34px;
    line-height: 34px;
    }
  }

  /* když je ultra nízká výška (některé iPhony/lišty), ještě víc zmenši */
  @media (orientation: landscape) and (max-height: 390px){
    h2{ font-size: 18px; }
    #toolbar{
      flex: 0 0 170px;
      width: 170px;
      max-width: 170px;
    }
    .colorBtn{ width: 34px; height: 34px; }
    .sizeBtn{ height: 32px; line-height: 32px; }
  }

</style>
</head>

<body>
<div id="header">
  <h2>ESP32 – canvas</h2>
  <p>Kresli prstem nebo myší. (Multi zařízení přes WS.)</p>
</div>

<div id="wrapper">
  <canvas id="canvas"></canvas>

  <div id="toolbar">
    <!-- Barvy -->
    <button class="colorBtn active" data-color="#00ff00" style="background:#00ff00;"></button>
    <button class="colorBtn" data-color="#ff0000" style="background:#ff0000;"></button>
    <button class="colorBtn" data-color="#0000ff" style="background:#0000ff;"></button>
    <button class="colorBtn" data-color="#ffff00" style="background:#ffff00;"></button>
    <button class="colorBtn" data-color="#000000" style="background:#000000;"></button>
    <button class="colorBtn" data-color="#ff00ff" style="background:#ff00ff;"></button>
    <button class="colorBtn" data-color="#ff8800" style="background:#ff8800;"></button>

    <!-- Nástroje -->
    <button class="sizeBtn active" id="penBtn" style="margin-top:6px; width:auto;">PERO</button>
    <button class="sizeBtn" id="eraserBtn" style="width:auto;">GUMA</button>

    <!-- Tloušťka pera -->
    <div class="sizeRow" style="margin-top:6px;">
      <button class="sizeBtn" id="sizeMinus">−</button>
      <div id="sizeValue">2</div>
      <button class="sizeBtn" id="sizePlus">+</button>
    </div>

    <button class="sizeBtn active" id="sizeReset" style="margin-top:4px; width:auto;">RESET</button>

    <button id="clearBtn">Vymazat</button>
  </div>
</div>

<script>
  // ===== HARD ANTI-ZOOM (Android/Samsung + iOS) =====
  document.addEventListener('touchstart', (e) => {
    if (e.touches && e.touches.length > 1) e.preventDefault();
  }, { passive:false });

  document.addEventListener('touchmove', (e) => {
    if (e.touches && e.touches.length > 1) e.preventDefault();
  }, { passive:false });

  let lastTouchEnd = 0;
  document.addEventListener('touchend', (e) => {
    const now = Date.now();
    if (now - lastTouchEnd <= 300) e.preventDefault();
    lastTouchEnd = now;
  }, { passive:false });

  document.addEventListener('gesturestart', e => e.preventDefault(), { passive:false });
  document.addEventListener('gesturechange', e => e.preventDefault(), { passive:false });
  document.addEventListener('gestureend', e => e.preventDefault(), { passive:false });

  document.addEventListener('wheel', (e) => {
    if (e.ctrlKey) e.preventDefault();
  }, { passive:false });

  const canvas  = document.getElementById('canvas');
  const ctx     = canvas.getContext('2d');
  const toolbar = document.getElementById('toolbar');
  const wrapper = document.getElementById('wrapper');

  // ===== logické rozměry canvasu (souřadnice, které posíláme na ESP) =====
  const CANVAS_W = 320, CANVAS_H = 240;

  // ===== MODEL (offscreen) canvas: "pravda" kresby v 320x240 =====
  const modelCanvas = document.createElement('canvas');
  modelCanvas.width = CANVAS_W;
  modelCanvas.height = CANVAS_H;
  const mctx = modelCanvas.getContext('2d');
  mctx.lineJoin = 'round';
  mctx.lineCap  = 'round';
  mctx.fillStyle = "#ffffff";
  mctx.fillRect(0, 0, CANVAS_W, CANVAS_H);

  let currentDpr = window.devicePixelRatio || 1;

  function renderFromModel() {
    ctx.clearRect(0, 0, CANVAS_W, CANVAS_H);
    ctx.drawImage(modelCanvas, 0, 0, CANVAS_W, CANVAS_H);
  }

  // ===== kreslení stav =====
  let drawing = false;
  let lastX = 0, lastY = 0;

  // ===== barva / tloušťka / nástroj =====
  let currentColor = '#00ff00';

  const DEFAULT_PEN_SIZE = 2;
  const MIN_PEN_SIZE = 1;
  const MAX_PEN_SIZE = 20;
  let penSize = DEFAULT_PEN_SIZE;

  let tool = "pen"; // "pen" | "eraser"

  // ===== clientId pro ignorování vlastních broadcast zpráv =====
  const clientId = Math.random().toString(16).slice(2);

  // ===== snapshot persistence =====
  const SNAP_KEY = "esp32_canvas_snapshot_v3";

  function saveSnapshotSoon(){
    if (saveSnapshotSoon._t) return;
    saveSnapshotSoon._t = setTimeout(() => {
      saveSnapshotSoon._t = null;
      try {
        const data = modelCanvas.toDataURL("image/png");
        localStorage.setItem(SNAP_KEY, data);
      } catch(e) {}
    }, 500);
  }

  function restoreFromLocalStorage(){
    try {
      const data = localStorage.getItem(SNAP_KEY);
      if (!data) return false;

      const img = new Image();
      img.onload = () => {
        mctx.fillStyle = "#ffffff";
        mctx.fillRect(0,0,CANVAS_W,CANVAS_H);
        mctx.drawImage(img, 0, 0, CANVAS_W, CANVAS_H);
        renderFromModel();
      };
      img.src = data;
      return true;
    } catch(e) {
      return false;
    }
  }

  // ===== layout/resize bez mazání (iOS safe: bere rozměry z wrapperu) =====
  function setupCanvas() {
    currentDpr = window.devicePixelRatio || 1;

    // dostupná velikost = reálně velikost wrapperu
    const wrapRect = wrapper.getBoundingClientRect();
    const tbRect = toolbar.getBoundingClientRect();

    const gap = 10;
    const padding = 16;

    const isPortrait = wrapRect.height >= wrapRect.width;

    let availW, availH;

    if (isPortrait) {
      // toolbar dole
      availW = wrapRect.width - padding;
      availH = wrapRect.height - tbRect.height - gap - padding;
    } else {
      // toolbar vpravo
      availW = wrapRect.width - tbRect.width - gap - padding;
      availH = wrapRect.height - padding;
    }

    // poměr 4:3
    let cssW = Math.min(availW, availH * (CANVAS_W / CANVAS_H));
    let cssH = cssW * (CANVAS_H / CANVAS_W);

    // minimální velikost
    const minW = 220;
    if (cssW < minW) {
      cssW = minW;
      cssH = cssW * (CANVAS_H / CANVAS_W);
    }

    canvas.style.width  = Math.round(cssW) + "px";
    canvas.style.height = Math.round(cssH) + "px";

    canvas.width  = Math.round(CANVAS_W * currentDpr);
    canvas.height = Math.round(CANVAS_H * currentDpr);

    ctx.setTransform(currentDpr, 0, 0, currentDpr, 0, 0);
    ctx.lineJoin = 'round';
    ctx.lineCap  = 'round';

    renderFromModel();
  }

  let resizeTimer = null;
  function scheduleResize(){
    clearTimeout(resizeTimer);
    resizeTimer = setTimeout(setupCanvas, 140);
  }

  setupCanvas();
  restoreFromLocalStorage();

  window.addEventListener('resize', scheduleResize);
  if (window.visualViewport) window.visualViewport.addEventListener('resize', scheduleResize);
  window.addEventListener('orientationchange', scheduleResize);

  // ===== UI barvy =====
  const colorButtons = document.querySelectorAll('.colorBtn');
  colorButtons.forEach(btn => {
    btn.addEventListener('click', () => {
      currentColor = btn.dataset.color;
      colorButtons.forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
    });
  });

  // ===== UI tloušťka =====
  const sizeValueEl = document.getElementById('sizeValue');
  const sizeMinus   = document.getElementById('sizeMinus');
  const sizePlus    = document.getElementById('sizePlus');
  const sizeReset   = document.getElementById('sizeReset');

  function updatePenSize(newSize) {
    penSize = Math.max(MIN_PEN_SIZE, Math.min(MAX_PEN_SIZE, newSize));
    sizeValueEl.textContent = penSize;
  }
  sizeMinus.addEventListener('click', () => updatePenSize(penSize - 1));
  sizePlus .addEventListener('click', () => updatePenSize(penSize + 1));
  sizeReset.addEventListener('click', () => updatePenSize(DEFAULT_PEN_SIZE));
  updatePenSize(DEFAULT_PEN_SIZE);

  // ===== UI nástroje =====
  const penBtn    = document.getElementById("penBtn");
  const eraserBtn = document.getElementById("eraserBtn");

  function setTool(t){
    tool = t;
    penBtn.classList.toggle("active", tool==="pen");
    eraserBtn.classList.toggle("active", tool==="eraser");
  }
  penBtn.addEventListener("click", ()=>setTool("pen"));
  eraserBtn.addEventListener("click", ()=>setTool("eraser"));

  function getDrawColor(){
    return (tool==="eraser") ? "#ffffff" : currentColor;
  }

  // ===== WS =====
  let socket;

  function connectWS() {
    socket = new WebSocket('ws://' + window.location.host + '/ws');
    socket.onopen = () => {
      console.log('WS OK');
      // požádej o synchronizaci od ostatních klientů
      sendMsg('sync_req:0,0:#000000:0:pen');
    };
    socket.onclose = () => setTimeout(connectWS, 1000);

    socket.onmessage = (ev) => {
      const msg = ev.data; // id|payload
      const pipe = msg.indexOf('|');
      if (pipe <= 0) return;

      const fromId = msg.substring(0, pipe);
      if (fromId === clientId) return;

      handleIncoming(msg.substring(pipe + 1), fromId);
    };
  }
  connectWS();

  function sendMsg(s) {
    if (socket && socket.readyState === WebSocket.OPEN) socket.send(clientId + '|' + s);
  }

  // payload: typ:x,y:barva:tloustka:tool
  function sendPoint(type, x, y) {
    sendMsg(
      type + ':' +
      Math.round(x) + ',' + Math.round(y) + ':' +
      currentColor + ':' + penSize + ':' + tool
    );
  }

  function getPos(e) {
    const rect = canvas.getBoundingClientRect();
    const clientX = (e.touches && e.touches.length) ? e.touches[0].clientX : e.clientX;
    const clientY = (e.touches && e.touches.length) ? e.touches[0].clientY : e.clientY;

    const x = (clientX - rect.left) * (CANVAS_W / rect.width);
    const y = (clientY - rect.top)  * (CANVAS_H / rect.height);

    return { x, y };
  }

  function drawDotLocal(x, y, color, size) {
    // model
    mctx.beginPath();
    mctx.arc(x, y, size / 2, 0, Math.PI * 2);
    mctx.fillStyle = color;
    mctx.fill();

    // visible
    ctx.beginPath();
    ctx.arc(x, y, size / 2, 0, Math.PI * 2);
    ctx.fillStyle = color;
    ctx.fill();
  }

  function drawLineLocal(x0, y0, x1, y1, color, size) {
    // model
    mctx.strokeStyle = color;
    mctx.lineWidth = size;
    mctx.beginPath();
    mctx.moveTo(x0, y0);
    mctx.lineTo(x1, y1);
    mctx.stroke();

    // visible
    ctx.strokeStyle = color;
    ctx.lineWidth = size;
    ctx.beginPath();
    ctx.moveTo(x0, y0);
    ctx.lineTo(x1, y1);
    ctx.stroke();
  }

  function startDraw(e) {
    e.preventDefault();
    const pos = getPos(e);
    drawing = true;
    lastX = pos.x; lastY = pos.y;

    drawDotLocal(pos.x, pos.y, getDrawColor(), penSize);
    sendPoint('start', pos.x, pos.y);
    saveSnapshotSoon();
  }

  function moveDraw(e) {
    if (!drawing) return;
    e.preventDefault();
    const pos = getPos(e);

    drawLineLocal(lastX, lastY, pos.x, pos.y, getDrawColor(), penSize);
    sendPoint('move', pos.x, pos.y);

    lastX = pos.x; lastY = pos.y;
    saveSnapshotSoon();
  }

  function endDraw(e) {
    if (!drawing) return;
    e.preventDefault();
    drawing = false;
    sendMsg('end:0,0:' + currentColor + ':' + penSize + ':' + tool);
    saveSnapshotSoon();
  }

  // myš
  canvas.addEventListener('mousedown', startDraw);
  canvas.addEventListener('mousemove', moveDraw);
  canvas.addEventListener('mouseup', endDraw);
  canvas.addEventListener('mouseleave', endDraw);

  // dotyk
  canvas.addEventListener('touchstart', startDraw, {passive:false});
  canvas.addEventListener('touchmove',  moveDraw,  {passive:false});
  canvas.addEventListener('touchend',   endDraw,   {passive:false});
  canvas.addEventListener('touchcancel',endDraw,   {passive:false});

  // clear
  document.getElementById('clearBtn').addEventListener('click', () => {
    mctx.fillStyle = "#ffffff";
    mctx.fillRect(0, 0, CANVAS_W, CANVAS_H);
    renderFromModel();
    saveSnapshotSoon();
    sendMsg('clear:0,0:#000000:0:pen');
  });

  // ===== PŘÍJEM CIZÍCH TAHŮ + SYNC =====
  let remoteDown = false;
  let rLastX = 0, rLastY = 0;

  function handleIncoming(payload, fromId){
    // ---- SYNC REQUEST ----
    if (payload.startsWith("sync_req:")) {
      try {
        const imgData = modelCanvas.toDataURL("image/png");
        // pošli cíleně konkrétnímu klientovi
        sendMsg("sync_img:" + fromId + ":" + imgData);
      } catch(e) {}
      return;
    }

    // ---- SYNC IMAGE ----
    if (payload.startsWith("sync_img:")) {
      // formát: sync_img:<targetId>:data:image/png;base64,...
      const p1 = payload.indexOf(':');
      const p2 = payload.indexOf(':', p1 + 1);
      if (p2 < 0) return;

      const targetId = payload.substring(p1 + 1, p2);
      if (targetId !== clientId) return;

      const dataUrl = payload.substring(p2 + 1);
      const img = new Image();
      img.onload = () => {
        mctx.fillStyle = "#ffffff";
        mctx.fillRect(0, 0, CANVAS_W, CANVAS_H);
        mctx.drawImage(img, 0, 0, CANVAS_W, CANVAS_H);
        renderFromModel();
        saveSnapshotSoon();
      };
      img.src = dataUrl;
      return;
    }

    // ---- STANDARD DRAW ----
    const parts = payload.split(':');
    if (parts.length < 4) return;

    const type = parts[0];

    const xy = parts[1].split(',');
    if (xy.length !== 2) return;
    const x = parseInt(xy[0], 10);
    const y = parseInt(xy[1], 10);

    const color = parts[2];
    const th = parseInt(parts[3], 10) || 1;
    const toolIn = parts[4] || "pen";
    const drawColor = (toolIn === "eraser") ? "#ffffff" : color;

    if (type === "clear") {
      mctx.fillStyle = "#ffffff";
      mctx.fillRect(0, 0, CANVAS_W, CANVAS_H);
      renderFromModel();
      remoteDown = false;
      saveSnapshotSoon();
      return;
    }

    if (type === "start") {
      remoteDown = true;
      rLastX = x; rLastY = y;
      drawDotLocal(x, y, drawColor, th);
      saveSnapshotSoon();
      return;
    }

    if (type === "move") {
      if (!remoteDown) {
        remoteDown = true;
        rLastX = x; rLastY = y;
        return;
      }
      drawLineLocal(rLastX, rLastY, x, y, drawColor, th);
      rLastX = x; rLastY = y;
      saveSnapshotSoon();
      return;
    }

    if (type === "end") {
      remoteDown = false;
      saveSnapshotSoon();
      return;
    }
  }
</script>
</body>
</html>
)rawliteral";
