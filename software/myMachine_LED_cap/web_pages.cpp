#include "web_pages.h"

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport"
      content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
<title>myMachine LED Cap</title>

<style>
  html, body {
    margin: 0; padding: 0; height: 100%;
    background:#222; color:#fff;
    font-family:sans-serif; text-align:center;
    overflow: hidden;
    touch-action: none;
    -webkit-text-size-adjust: 100%;
  }
  body { display:flex; flex-direction:column; min-height:100vh; }
  #header { flex: 0 0 auto; padding: 6px 8px 0; }
  h2 { margin: 6px 0 4px; }
  p  { margin: 6px 0 0; font-size: 14px; color:#ddd; }

  #wrapper {
    flex: 1 1 auto; min-height: 0;
    display:flex; justify-content:center; align-items:center;
    gap:10px; padding: 8px; box-sizing: border-box;
    max-width: 100vw; flex-direction: column;
  }
  #canvas { background:#fff; border:1px solid #555; touch-action:none; display:block; }

  #toolbar {
    display:flex; flex-direction: row; flex-wrap: wrap;
    gap:6px; justify-content:center; align-items:center;
    width: 100%; max-width: 720px;
  }

  .colorBtn {
    width:36px; height:36px; border-radius:50%;
    border:2px solid #444; padding:0; cursor:pointer;
  }
  .colorBtn.active { border-color:#fff; box-shadow:0 0 5px #fff; }

  .sizeRow { display:flex; gap:6px; justify-content:center; align-items:center; width: 100%; }

  .sizeBtn {
    width:36px; height:32px; border-radius:10px;
    border:2px solid #444; background:#333; color:#fff; cursor:pointer;
  }
  .sizeBtn.active { border-color:#fff; box-shadow:0 0 5px #fff; }

  #sizeValue { color:#fff; font-weight:bold; text-align:center; min-width:32px; line-height:32px; }
  #clearBtn  { margin-top:6px; padding:6px 10px; cursor:pointer; }

  /* ── Přepínač módů ── */
  .modeSection { margin-top:10px; width:100%; }
  .modeLabel   { color:#aaa; font-size:11px; margin-bottom:4px; text-transform:uppercase; }
  .modeRow     { display:flex; gap:6px; justify-content:center; flex-wrap:wrap; }
  .modeBtn {
    padding:6px 10px; border-radius:8px;
    border:2px solid #444; background:#333; color:#fff;
    cursor:pointer; font-size:12px; font-weight:bold;
    -webkit-appearance:none; appearance:none;
  }
  .modeBtn.active { border-color:#fff; background:#555; box-shadow:0 0 6px #fff; }

  @media (orientation: landscape) {
    #wrapper { flex-direction: row; align-items: stretch; width: 100%; }
    #canvas  { flex: 1 1 auto; }
    #toolbar {
      flex: 0 0 190px; width: 190px; max-width: 190px;
      flex-direction: column; flex-wrap: nowrap; align-items:center;
      max-height: 100%; overflow-y: auto;
      -webkit-overflow-scrolling: touch;
      padding-bottom: 8px; box-sizing: border-box;
    }
    .sizeRow  { width: auto; }
    #header   { padding: 2px 8px 0; }
    h2        { margin: 2px 0 2px; font-size: 22px; }
    #header p { display: none; }
    #wrapper  { padding: 4px 6px; gap: 8px; }
    .colorBtn { width: 38px; height: 38px; border-radius: 50%; }
    .sizeBtn  { height: 34px; line-height: 34px; }
    .modeRow  { flex-direction: column; align-items: stretch; }
    .modeBtn  { width: 100%; }
  }
  @media (max-width: 420px){ .colorBtn { width:32px; height:32px; } }
  @media (orientation: landscape) and (max-height: 390px){
    h2 { font-size: 18px; }
    #toolbar { flex: 0 0 170px; width: 170px; max-width: 170px; }
    .colorBtn{ width: 34px; height: 34px; }
    .sizeBtn { height: 32px; line-height: 32px; }
  }
  button { -webkit-appearance: none; appearance: none; }
  #toolbar, #toolbar * { flex-shrink: 0; }
</style>
</head>

<body>
<div id="header">
  <h2>myMachine LED Cap</h2>
  <p>Kresli prstem nebo myší.</p>
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

    <!-- Mody -->
    <div class="modeSection">
      <div class="modeLabel">Mód</div>
      <div class="modeRow">
        <button class="modeBtn active" id="modeDrawing" onclick="setMode('drawing')">KRESLENI</button>
        <button class="modeBtn"        id="modePolice"  onclick="setMode('police')">POLICIE</button>
        <button class="modeBtn"        id="modeAccel"   onclick="setMode('accel')">ACCEL</button>
      </div>
    </div>
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

  // UI barvy
  document.querySelectorAll('.colorBtn').forEach(btn => {
    btn.addEventListener('click', () => {
      currentColor = btn.dataset.color;
      document.querySelectorAll('.colorBtn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
    });
  });

  // UI tloušťka
  const sizeValueEl = document.getElementById('sizeValue');
  function updatePenSize(s){ penSize = Math.max(MIN_PEN_SIZE, Math.min(MAX_PEN_SIZE, s)); sizeValueEl.textContent = penSize; }
  document.getElementById('sizeMinus').addEventListener('click', () => updatePenSize(penSize-1));
  document.getElementById('sizePlus') .addEventListener('click', () => updatePenSize(penSize+1));
  document.getElementById('sizeReset').addEventListener('click', () => updatePenSize(DEFAULT_PEN_SIZE));
  updatePenSize(DEFAULT_PEN_SIZE);

  // UI nástroj
  function setTool(t){ tool=t; document.getElementById('penBtn').classList.toggle('active',t==='pen'); document.getElementById('eraserBtn').classList.toggle('active',t==='eraser'); }
  document.getElementById('penBtn')   .addEventListener('click', ()=>setTool('pen'));
  document.getElementById('eraserBtn').addEventListener('click', ()=>setTool('eraser'));
  function getDrawColor(){ return (tool==='eraser') ? '#ffffff' : currentColor; }

  // Přepínání módů
  function setMode(mode) {
    fetch('/mode', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'mode='+mode})
      .then(() => {
        document.querySelectorAll('.modeBtn').forEach(b => b.classList.remove('active'));
        const id = 'mode' + mode.charAt(0).toUpperCase() + mode.slice(1);
        const el = document.getElementById(id);
        if (el) el.classList.add('active');
      })
      .catch(() => {});
  }

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
