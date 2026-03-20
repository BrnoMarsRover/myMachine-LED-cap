#include "include/web_pages.h"

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport"
      content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
<title>myMachine LED Cap</title>

<style>
  *, *::before, *::after { box-sizing: border-box; }
  html, body {
    margin: 0; padding: 0; height: 100%;
    background: #222; color: #fff;
    font-family: sans-serif; text-align: center;
    overflow: hidden;
    touch-action: none;
    -webkit-text-size-adjust: 100%;
  }
  body { display: flex; flex-direction: column; }
  button { -webkit-appearance: none; appearance: none; }

  /* ── Tab bar ── */
  #tabbar {
    flex: 0 0 44px;
    display: flex;
    background: #1a1a1a;
    border-bottom: 2px solid #444;
  }
  .tab {
    flex: 1;
    border: none; border-bottom: 3px solid transparent;
    background: transparent; color: #777;
    font-size: 13px; font-weight: bold; letter-spacing: 0.5px;
    cursor: pointer; padding: 0;
    transition: color 0.15s;
  }
  .tab.active { color: #fff; border-bottom-color: #fff; }

  /* ── Tab contents ── */
  .tab-content { display: none; flex: 1; overflow: hidden; }
  .tab-content.active { display: flex; flex-direction: column; }

  /* ── Drawing tab ── */
  #wrapper {
    flex: 1; min-height: 0;
    display: flex; justify-content: center; align-items: center;
    gap: 10px; padding: 8px;
    max-width: 100vw; flex-direction: column;
  }
  #canvas { background: #fff; border: 1px solid #555; touch-action: none; display: block; }

  #toolbar {
    display: flex; flex-direction: row; flex-wrap: wrap;
    gap: 6px; justify-content: center; align-items: center;
    width: 100%; max-width: 720px;
  }

  .colorBtn {
    width: 36px; height: 36px; border-radius: 50%;
    border: 2px solid #444; padding: 0; cursor: pointer;
  }
  .colorBtn.active { border-color: #fff; box-shadow: 0 0 5px #fff; }

  .sizeRow { display: flex; gap: 6px; justify-content: center; align-items: center; width: 100%; }

  .sizeBtn {
    width: 36px; height: 32px; border-radius: 10px;
    border: 2px solid #444; background: #333; color: #fff; cursor: pointer;
  }
  .sizeBtn.active { border-color: #fff; box-shadow: 0 0 5px #fff; }

  #sizeValue { color: #fff; font-weight: bold; text-align: center; min-width: 32px; line-height: 32px; }
  #clearBtn  { margin-top: 6px; padding: 6px 10px; cursor: pointer; }

  /* ── Mood selector ── */
  #moodRow {
    display: flex; align-items: center; justify-content: center;
    gap: 6px; width: 100%; margin-top: 8px;
    border-top: 1px solid #444; padding-top: 8px;
  }
  .moodArrow {
    width: 32px; height: 32px; border-radius: 8px;
    border: 2px solid #444; background: #333; color: #fff;
    cursor: pointer; font-size: 16px; padding: 0; line-height: 28px;
  }
  #moodDot {
    width: 22px; height: 22px; border-radius: 50%;
    border: 2px solid #666; flex-shrink: 0;
  }
  #moodName { font-size: 12px; min-width: 82px; text-align: center; }

  /* ── Battery indicator ── */
  #battIndicator {
    flex: 0 0 auto;
    display: flex; align-items: center;
    padding: 0 10px;
    font-size: 12px; color: #aaa;
    white-space: nowrap; gap: 4px;
  }
  #battIndicator.low { color: #ff4444; }

  /* ── SOS / Blinkr placeholder tabs ── */
  .empty-tab {
    flex: 1; display: flex; flex-direction: column;
    align-items: center; justify-content: center; gap: 12px;
  }
  .empty-tab .icon { font-size: 52px; }
  .empty-tab .title { font-size: 20px; font-weight: bold; }
  .empty-tab .sub   { font-size: 13px; color: #aaa; }

  /* ── Landscape ── */
  @media (orientation: landscape) {
    #wrapper { flex-direction: row; align-items: stretch; width: 100%; }
    #canvas  { flex: 1 1 auto; }
    #toolbar {
      flex: 0 0 190px; width: 190px; max-width: 190px;
      flex-direction: column; flex-wrap: nowrap; align-items: center;
      max-height: 100%; overflow-y: auto;
      -webkit-overflow-scrolling: touch;
      padding-bottom: 8px;
    }
    .sizeRow  { width: auto; }
    #wrapper  { padding: 4px 6px; gap: 8px; }
    .colorBtn { width: 38px; height: 38px; }
    .sizeBtn  { height: 34px; line-height: 34px; }
    #moodRow  { flex-direction: row; }
  }
  @media (max-width: 420px){ .colorBtn { width: 32px; height: 32px; } }
  @media (orientation: landscape) and (max-height: 390px){
    #toolbar { flex: 0 0 170px; width: 170px; max-width: 170px; }
    .colorBtn { width: 34px; height: 34px; }
    .sizeBtn  { height: 32px; line-height: 32px; }
  }
  #toolbar, #toolbar * { flex-shrink: 0; }
</style>
</head>

<body>

<!-- Tab bar -->
<div id="tabbar">
  <button class="tab active" id="tab-drawing" onclick="switchTab('drawing')">KRESLENÍ</button>
  <button class="tab"        id="tab-sos"     onclick="switchTab('sos')">SOS</button>
  <button class="tab"        id="tab-blinkr"  onclick="switchTab('blinkr')">BLINKR</button>
  <div id="battIndicator"><span id="battPct">--</span>%</div>
</div>

<!-- Kreslení -->
<div id="tab-content-drawing" class="tab-content active">
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
      <button class="sizeBtn active" id="penBtn"    style="margin-top:6px; width:auto;">PERO</button>
      <button class="sizeBtn"        id="eraserBtn" style="width:auto;">GUMA</button>

      <!-- Tloušťka -->
      <div class="sizeRow" style="margin-top:6px;">
        <button class="sizeBtn" id="sizeMinus">−</button>
        <div id="sizeValue">2</div>
        <button class="sizeBtn" id="sizePlus">+</button>
      </div>
      <button class="sizeBtn active" id="sizeReset" style="margin-top:4px; width:auto;">RESET</button>

      <button id="clearBtn">Vymazat</button>

      <!-- Nálada -->
      <div id="moodRow">
        <button class="moodArrow" onclick="prevMood()">&#9664;</button>
        <div id="moodDot"></div>
        <span id="moodName"></span>
        <button class="moodArrow" onclick="nextMood()">&#9654;</button>
      </div>
    </div>
  </div>
</div>

<!-- SOS -->
<div id="tab-content-sos" class="tab-content">
  <div class="empty-tab">
    <div class="icon">&#128680;</div>
    <div class="title">SOS</div>
    <div class="sub">LED pásek bliká modro&#8209;červeně</div>
  </div>
</div>

<!-- Blinkr -->
<div id="tab-content-blinkr" class="tab-content">
  <div class="empty-tab">
    <div class="icon">&#128661;</div>
    <div class="title">Blinkr</div>
    <div class="sub">Nakloň čepici pro aktivaci blinkru</div>
  </div>
</div>

<script>
  // Anti-zoom
  document.addEventListener('touchstart', e => { if (e.touches.length > 1) e.preventDefault(); }, {passive:false});
  document.addEventListener('touchmove',  e => { if (e.touches.length > 1) e.preventDefault(); }, {passive:false});
  let lastTouchEnd = 0;
  document.addEventListener('touchend', e => { const now = Date.now(); if (now - lastTouchEnd <= 300) e.preventDefault(); lastTouchEnd = now; }, {passive:false});
  document.addEventListener('gesturestart',  e => e.preventDefault(), {passive:false});
  document.addEventListener('gesturechange', e => e.preventDefault(), {passive:false});
  document.addEventListener('gestureend',    e => e.preventDefault(), {passive:false});
  document.addEventListener('wheel', e => { if (e.ctrlKey) e.preventDefault(); }, {passive:false});

  // ── Baterie ──────────────────────────────────────────────────
  function updateBattery() {
    fetch('/battery').then(r => r.text()).then(pct => {
      const n = parseInt(pct, 10);
      document.getElementById('battPct').textContent = n;
      const el = document.getElementById('battIndicator');
      el.classList.toggle('low', n < 20);
    }).catch(() => {});
  }
  updateBattery();
  setInterval(updateBattery, 30000);

  // ── Záložky ──────────────────────────────────────────────────
  function switchTab(mode) {
    document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    document.getElementById('tab-content-' + mode).classList.add('active');
    document.getElementById('tab-' + mode).classList.add('active');
    fetch('/mode', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'mode='+mode}).catch(()=>{});
    if (mode === 'drawing') setTimeout(setupCanvas, 50);
  }

  // ── Nálady ───────────────────────────────────────────────────
  const MOODS = [
    {name: "Smutný",     color: "#0000ff"},
    {name: "Veselý",     color: "#ffff00"},
    {name: "Naštvaný",   color: "#ff0000"},
    {name: "Překvapený", color: "#ff8800"},
    {name: "Vyděšený",   color: "#00ff00"},
    {name: "Nadšený",    color: "#aa00ff"},
  ];
  let currentMood = 0;

  function updateMoodUI() {
    document.getElementById('moodDot').style.background  = MOODS[currentMood].color;
    document.getElementById('moodDot').style.borderColor = MOODS[currentMood].color;
    document.getElementById('moodName').textContent = MOODS[currentMood].name;
  }
  function setMood(i) {
    currentMood = i;
    updateMoodUI();
    fetch('/mood', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'mood='+i}).catch(()=>{});
  }
  function prevMood() { setMood((currentMood + MOODS.length - 1) % MOODS.length); }
  function nextMood() { setMood((currentMood + 1) % MOODS.length); }
  updateMoodUI();

  // ── Canvas ───────────────────────────────────────────────────
  const canvas  = document.getElementById('canvas');
  const ctx     = canvas.getContext('2d');
  const toolbar = document.getElementById('toolbar');
  const wrapper = document.getElementById('wrapper');

  const CANVAS_W = 320, CANVAS_H = 240;

  const modelCanvas = document.createElement('canvas');
  modelCanvas.width = CANVAS_W; modelCanvas.height = CANVAS_H;
  const mctx = modelCanvas.getContext('2d');
  mctx.lineJoin = 'round'; mctx.lineCap = 'round';
  mctx.fillStyle = "#ffffff"; mctx.fillRect(0, 0, CANVAS_W, CANVAS_H);

  let currentDpr = window.devicePixelRatio || 1;
  function renderFromModel() { ctx.clearRect(0,0,CANVAS_W,CANVAS_H); ctx.drawImage(modelCanvas, 0,0,CANVAS_W,CANVAS_H); }

  let drawing = false, lastX = 0, lastY = 0;
  let currentColor = '#00ff00';
  const DEFAULT_PEN_SIZE = 2, MIN_PEN_SIZE = 1, MAX_PEN_SIZE = 20;
  let penSize = DEFAULT_PEN_SIZE;
  let tool = "pen";
  const clientId = Math.random().toString(16).slice(2);
  const SNAP_KEY = "myMachine_canvas_v1";

  function saveSnapshotSoon(){
    if (saveSnapshotSoon._t) return;
    saveSnapshotSoon._t = setTimeout(() => {
      saveSnapshotSoon._t = null;
      try { localStorage.setItem(SNAP_KEY, modelCanvas.toDataURL("image/png")); } catch(e) {}
    }, 500);
  }
  function restoreFromLocalStorage(){
    try {
      const data = localStorage.getItem(SNAP_KEY);
      if (!data) return;
      const img = new Image();
      img.onload = () => { mctx.fillStyle="#ffffff"; mctx.fillRect(0,0,CANVAS_W,CANVAS_H); mctx.drawImage(img,0,0,CANVAS_W,CANVAS_H); renderFromModel(); };
      img.src = data;
    } catch(e) {}
  }

  function setupCanvas() {
    currentDpr = window.devicePixelRatio || 1;
    const wrapRect = wrapper.getBoundingClientRect();
    const tbRect   = toolbar.getBoundingClientRect();
    if (wrapRect.width === 0) return;
    const gap = 10, padding = 16;
    const isPortrait = wrapRect.height >= wrapRect.width;
    let availW = isPortrait ? wrapRect.width - padding : wrapRect.width - tbRect.width - gap - padding;
    let availH = isPortrait ? wrapRect.height - tbRect.height - gap - padding : wrapRect.height - padding;
    let cssW = Math.min(availW, availH * (CANVAS_W / CANVAS_H));
    let cssH = cssW * (CANVAS_H / CANVAS_W);
    if (cssW < 220) { cssW = 220; cssH = cssW * (CANVAS_H / CANVAS_W); }
    canvas.style.width  = Math.round(cssW) + "px";
    canvas.style.height = Math.round(cssH) + "px";
    canvas.width  = Math.round(CANVAS_W * currentDpr);
    canvas.height = Math.round(CANVAS_H * currentDpr);
    ctx.setTransform(currentDpr,0,0,currentDpr,0,0);
    ctx.lineJoin='round'; ctx.lineCap='round';
    renderFromModel();
  }
  let resizeTimer = null;
  function scheduleResize(){ clearTimeout(resizeTimer); resizeTimer = setTimeout(setupCanvas, 140); }
  setupCanvas(); restoreFromLocalStorage();
  window.addEventListener('resize', scheduleResize);
  if (window.visualViewport) window.visualViewport.addEventListener('resize', scheduleResize);
  window.addEventListener('orientationchange', scheduleResize);

  // Barvy
  document.querySelectorAll('.colorBtn').forEach(btn => {
    btn.addEventListener('click', () => {
      currentColor = btn.dataset.color;
      document.querySelectorAll('.colorBtn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
    });
  });

  // Tloušťka
  const sizeValueEl = document.getElementById('sizeValue');
  function updatePenSize(s){ penSize = Math.max(MIN_PEN_SIZE, Math.min(MAX_PEN_SIZE, s)); sizeValueEl.textContent = penSize; }
  document.getElementById('sizeMinus').addEventListener('click', () => updatePenSize(penSize-1));
  document.getElementById('sizePlus') .addEventListener('click', () => updatePenSize(penSize+1));
  document.getElementById('sizeReset').addEventListener('click', () => updatePenSize(DEFAULT_PEN_SIZE));
  updatePenSize(DEFAULT_PEN_SIZE);

  // Nástroj
  function setTool(t){ tool=t; document.getElementById('penBtn').classList.toggle('active',t==='pen'); document.getElementById('eraserBtn').classList.toggle('active',t==='eraser'); }
  document.getElementById('penBtn')   .addEventListener('click', ()=>setTool('pen'));
  document.getElementById('eraserBtn').addEventListener('click', ()=>setTool('eraser'));
  function getDrawColor(){ return (tool==='eraser') ? '#ffffff' : currentColor; }

  // WS
  let socket;
  function connectWS(){
    socket = new WebSocket('ws://' + window.location.host + '/ws');
    socket.onopen  = () => { console.log('WS OK'); sendMsg('sync_req:0,0:#000000:0:pen'); };
    socket.onclose = () => setTimeout(connectWS, 1000);
    socket.onmessage = ev => {
      const msg = ev.data;
      const pipe = msg.indexOf('|');
      if (pipe <= 0) return;
      const fromId = msg.substring(0, pipe);
      if (fromId === clientId) return;
      handleIncoming(msg.substring(pipe+1), fromId);
    };
  }
  connectWS();

  function sendMsg(s){ if (socket && socket.readyState===WebSocket.OPEN) socket.send(clientId+'|'+s); }
  function sendPoint(type,x,y){ sendMsg(type+':'+Math.round(x)+','+Math.round(y)+':'+currentColor+':'+penSize+':'+tool); }

  function getPos(e){
    const rect=canvas.getBoundingClientRect();
    const cx=(e.touches&&e.touches.length)?e.touches[0].clientX:e.clientX;
    const cy=(e.touches&&e.touches.length)?e.touches[0].clientY:e.clientY;
    return {x:(cx-rect.left)*(CANVAS_W/rect.width), y:(cy-rect.top)*(CANVAS_H/rect.height)};
  }
  function drawDotLocal(x,y,color,size){ mctx.beginPath(); mctx.arc(x,y,size/2,0,Math.PI*2); mctx.fillStyle=color; mctx.fill(); ctx.beginPath(); ctx.arc(x,y,size/2,0,Math.PI*2); ctx.fillStyle=color; ctx.fill(); }
  function drawLineLocal(x0,y0,x1,y1,color,size){ mctx.strokeStyle=color; mctx.lineWidth=size; mctx.beginPath(); mctx.moveTo(x0,y0); mctx.lineTo(x1,y1); mctx.stroke(); ctx.strokeStyle=color; ctx.lineWidth=size; ctx.beginPath(); ctx.moveTo(x0,y0); ctx.lineTo(x1,y1); ctx.stroke(); }

  function startDraw(e){ e.preventDefault(); const pos=getPos(e); drawing=true; lastX=pos.x; lastY=pos.y; drawDotLocal(pos.x,pos.y,getDrawColor(),penSize); sendPoint('start',pos.x,pos.y); saveSnapshotSoon(); }
  function moveDraw(e){ if(!drawing) return; e.preventDefault(); const pos=getPos(e); drawLineLocal(lastX,lastY,pos.x,pos.y,getDrawColor(),penSize); sendPoint('move',pos.x,pos.y); lastX=pos.x; lastY=pos.y; saveSnapshotSoon(); }
  function endDraw(e){ if(!drawing) return; e.preventDefault(); drawing=false; sendMsg('end:0,0:'+currentColor+':'+penSize+':'+tool); saveSnapshotSoon(); }

  canvas.addEventListener('mousedown', startDraw);
  canvas.addEventListener('mousemove', moveDraw);
  canvas.addEventListener('mouseup',   endDraw);
  canvas.addEventListener('mouseleave',endDraw);
  canvas.addEventListener('touchstart', startDraw, {passive:false});
  canvas.addEventListener('touchmove',  moveDraw,  {passive:false});
  canvas.addEventListener('touchend',   endDraw,   {passive:false});
  canvas.addEventListener('touchcancel',endDraw,   {passive:false});

  document.getElementById('clearBtn').addEventListener('click', () => {
    mctx.fillStyle="#ffffff"; mctx.fillRect(0,0,CANVAS_W,CANVAS_H);
    renderFromModel(); saveSnapshotSoon(); sendMsg('clear:0,0:#000000:0:pen');
  });

  // Příjem tahů
  let remoteDown=false, rLastX=0, rLastY=0;
  function handleIncoming(payload, fromId){
    if (payload.startsWith("sync_req:")) { try { sendMsg("sync_img:"+fromId+":"+modelCanvas.toDataURL("image/png")); } catch(e){} return; }
    if (payload.startsWith("sync_img:")) {
      const p1=payload.indexOf(':'), p2=payload.indexOf(':',p1+1);
      if (p2<0) return;
      if (payload.substring(p1+1,p2)!==clientId) return;
      const img=new Image(); img.onload=()=>{ mctx.fillStyle="#ffffff"; mctx.fillRect(0,0,CANVAS_W,CANVAS_H); mctx.drawImage(img,0,0,CANVAS_W,CANVAS_H); renderFromModel(); saveSnapshotSoon(); }; img.src=payload.substring(p2+1);
      return;
    }
    const parts=payload.split(':');
    if (parts.length<4) return;
    const type=parts[0], xy=parts[1].split(',');
    if (xy.length!==2) return;
    const x=parseInt(xy[0],10), y=parseInt(xy[1],10);
    const color=parts[2], th=parseInt(parts[3],10)||1, toolIn=parts[4]||"pen";
    const drawColor=(toolIn==="eraser")?"#ffffff":color;
    if (type==="clear"){ mctx.fillStyle="#ffffff"; mctx.fillRect(0,0,CANVAS_W,CANVAS_H); renderFromModel(); remoteDown=false; saveSnapshotSoon(); return; }
    if (type==="start"){ remoteDown=true; rLastX=x; rLastY=y; drawDotLocal(x,y,drawColor,th); saveSnapshotSoon(); return; }
    if (type==="move"){ if(!remoteDown){remoteDown=true;rLastX=x;rLastY=y;return;} drawLineLocal(rLastX,rLastY,x,y,drawColor,th); rLastX=x; rLastY=y; saveSnapshotSoon(); return; }
    if (type==="end"){ remoteDown=false; saveSnapshotSoon(); return; }
  }
</script>
</body>
</html>
)rawliteral";
