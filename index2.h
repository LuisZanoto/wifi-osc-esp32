#ifndef INDEX_H
#define INDEX_H

static const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
 <meta charset="UTF-8">
 <meta name="viewport" content="width=device-width, initial-scale=1.0">
 <title>Osci_ESP32 8-bit Turbo</title>
 <style>
 :root {
 --bg: #070b12;
 --bg-2: #0d131d;
 --panel: #121a26;
 --panel-2: #0e1520;
 --border: #21304a;
 --border-hi: #2f4a6e;
 --text: #cfe3ff;
 --text-dim: #6b829e;
 --accent: #38bdf8;
 --trace: #3dff9a;
 --trig: #ffb454;
 --danger: #ff5470;
 --ok: #3dff9a;
 --radius: 14px;
 --shadow: 0 10px 30px -12px #000a;
 }
 * { box-sizing: border-box; }
 html, body { height: 100%; }
 body {
 margin: 0;
 background:
 radial-gradient(1200px 600px at 50% -10%, #10233a 0%, transparent 60%),
 linear-gradient(180deg, var(--bg-2), var(--bg));
 color: var(--text);
 font-family: 'Segoe UI', system-ui, -apple-system, Roboto, sans-serif;
 -webkit-font-smoothing: antialiased;
 min-height: 100%;
 padding: 18px clamp(10px, 3vw, 32px) 40px;
 }
 .topbar {
 max-width: 1180px;
 margin: 0 auto 16px;
 display: flex;
 align-items: center;
 justify-content: space-between;
 gap: 16px;
 flex-wrap: wrap;
 }
 .brand { display: flex; align-items: center; gap: 12px; }
 .brand .logo {
 width: 40px; height: 40px; border-radius: 11px;
 display: grid; place-items: center;
 background: linear-gradient(145deg, #0f2740, #0a1626);
 border: 1px solid var(--border);
 box-shadow: inset 0 0 14px #0af3, var(--shadow);
 }
 .brand .logo svg { width: 24px; height: 24px; }
 .brand h1 { margin: 0; font-size: 1.25rem; font-weight: 700; letter-spacing: .5px; }
 .brand h1 span { color: var(--accent); }
 .brand .sub { margin: 2px 0 0; font-size: .72rem; color: var(--text-dim); letter-spacing: 2px; text-transform: uppercase; }
 .status {
 display: flex; align-items: center; gap: 10px;
 background: var(--panel); border: 1px solid var(--border);
 padding: 8px 14px; border-radius: 999px;
 font-size: .82rem; box-shadow: var(--shadow);
 }
 .led {
 width: 11px; height: 11px; border-radius: 50%;
 background: var(--text-dim); position: relative;
 transition: background .3s, box-shadow .3s;
 }
 .led.on { background: var(--ok); box-shadow: 0 0 10px var(--ok), 0 0 4px var(--ok); animation: pulse 1.6s infinite; }
 .led.off { background: var(--danger); box-shadow: 0 0 10px var(--danger); }
 @keyframes pulse { 0%,100%{opacity:1} 50%{opacity:.45} }
 #statusText { color: var(--text-dim); }

 .layout { max-width: 1180px; margin: 0 auto; display: flex; flex-direction: column; gap: 16px; }

 .screen-wrap {
 position: relative;
 background: linear-gradient(160deg, #0a1220, #060a12);
 border: 1px solid var(--border);
 border-radius: var(--radius);
 padding: 14px;
 box-shadow: var(--shadow), inset 0 0 60px #0006;
 }
 .screen {
 position: relative;
 border-radius: 10px; overflow: hidden;
 background: #030507; border: 1px solid #16283b;
 box-shadow: inset 0 0 40px #000a, 0 0 0 1px #0af1;
 }
 canvas { display: block; width: 100%; height: auto; }

 .measbar {
 display: grid;
 grid-template-columns: repeat(7, 1fr);
 gap: 10px; margin-top: 12px;
 }
 .meas {
 background: var(--panel-2); border: 1px solid var(--border);
 border-radius: 10px; padding: 8px 10px; text-align: left;
 }
 .meas .k { font-size: .64rem; letter-spacing: 1.5px; text-transform: uppercase; color: var(--text-dim); }
 .meas .v { font-family: 'Cascadia Code', 'Consolas', monospace; font-size: 1.02rem; font-weight: 700; color: var(--trace); margin-top: 2px; }
 .meas.trig .v { color: var(--trig); }
 .meas.battery .v { color: var(--accent); }
 .meas.dac .v { color: var(--accent); }

 .meas.dac select {
 width: 100%;
 background: transparent;
 border: none;
 color: var(--accent);
 font-family: 'Cascadia Code', 'Consolas', monospace;
 font-size: 1.02rem;
 font-weight: 700;
 padding: 2px 0 0 0;
 margin: 0;
 cursor: pointer;
 outline: none;
 -webkit-appearance: none;
 appearance: none;
 }
 .meas.dac select option {
 background: #0a1826;
 color: var(--text);
 font-size: .9rem;
 }
 .meas.dac::after {
 content: '▾';
 position: relative;
 float: right;
 top: -18px;
 color: var(--text-dim);
 pointer-events: none;
 font-size: .7rem;
 }

 .panels {
 display: grid;
 grid-template-columns: repeat(auto-fit, minmax(230px, 1fr));
 gap: 14px;
 }
 .card {
 background: var(--panel); border: 1px solid var(--border);
 border-radius: var(--radius); padding: 16px;
 box-shadow: var(--shadow);
 }
 .card h2 {
 margin: 0 0 14px; font-size: .74rem;
 letter-spacing: 2px; text-transform: uppercase;
 color: var(--accent); display: flex; align-items: center; gap: 8px;
 }
 .card h2::before { content:''; width: 8px; height: 8px; border-radius: 2px; background: var(--accent); box-shadow: 0 0 8px var(--accent); }

 .ctrl { margin-bottom: 16px; }
 .ctrl:last-child { margin-bottom: 0; }
 .ctrl-head { display: flex; justify-content: space-between; align-items: baseline; margin-bottom: 7px; }
 .ctrl-head label { font-size: .82rem; color: var(--text); }
 .ctrl-head .val {
 font-family: 'Cascadia Code', 'Consolas', monospace;
 font-size: .8rem; color: var(--accent);
 background: #0a1826; border: 1px solid var(--border);
 padding: 1px 8px; border-radius: 6px; min-width: 54px; text-align: center;
 }

 select {
 width: 100%; background: #0a1826; border: 1px solid var(--border);
 color: var(--text); padding: 8px 12px; border-radius: 8px;
 font-size: .85rem; outline: none; cursor: pointer;
 }

 input[type=range] {
 -webkit-appearance: none; appearance: none;
 width: 100%; height: 6px; border-radius: 999px;
 background: linear-gradient(90deg, var(--accent) 0%, #16324a 0%);
 outline: none; cursor: pointer;
 }
 input[type=range]::-webkit-slider-thumb {
 -webkit-appearance: none; appearance: none;
 width: 18px; height: 18px; border-radius: 50%;
 background: radial-gradient(circle at 35% 30%, #eaf6ff, var(--accent));
 border: 2px solid #0a1826; box-shadow: 0 0 10px var(--accent);
 transition: transform .12s;
 }
 input[type=range]::-webkit-slider-thumb:hover { transform: scale(1.15); }
 input[type=range]::-moz-range-thumb {
 width: 16px; height: 16px; border-radius: 50%;
 background: var(--accent); border: 2px solid #0a1826;
 box-shadow: 0 0 10px var(--accent); cursor: pointer;
 }
 input[type=range]::-moz-range-track { height: 6px; border-radius: 999px; background: #16324a; }

 .switch-row { display: flex; align-items: center; justify-content: space-between; }
 .switch { position: relative; display: inline-block; width: 48px; height: 26px; }
 .switch input { opacity: 0; width: 0; height: 0; }
 .slider-sw {
 position: absolute; inset: 0; cursor: pointer;
 background: #16324a; border: 1px solid var(--border);
 border-radius: 999px; transition: .25s;
 }
 .slider-sw::before {
 content: ''; position: absolute; height: 18px; width: 18px; left: 3px; top: 3px;
 background: #cfe3ff; border-radius: 50%; transition: .25s;
 }
 .switch input:checked + .slider-sw { background: linear-gradient(90deg, #0e5a3a, var(--trace)); border-color: var(--trace); }
 .switch input:checked + .slider-sw::before { transform: translateX(22px); background: #061a10; }

 .btn {
 width: 100%;
 display: flex; align-items: center; justify-content: center; gap: 10px;
 font-size: 1rem; font-weight: 700; letter-spacing: .5px;
 padding: 14px; border-radius: 12px; cursor: pointer;
 border: 1px solid var(--trace);
 color: #04140c; background: linear-gradient(180deg, #52ffb0, #16d47f);
 box-shadow: 0 0 18px -4px var(--trace); transition: .18s;
 }
 .btn:hover { filter: brightness(1.08); transform: translateY(-1px); }
 .btn:active { transform: translateY(0); }
 .btn.stopped {
 border-color: var(--danger); color: #fff;
 background: linear-gradient(180deg, #ff6b86, #e23057);
 box-shadow: 0 0 18px -4px var(--danger);
 }
 .btn .dot { width: 10px; height: 10px; border-radius: 50%; background: #04140c; }
 .btn.stopped .dot { border-radius: 2px; background: #fff; }

 .hint { font-size: .72rem; color: var(--text-dim); margin-top: 10px; line-height: 1.5; }

 @media (max-width: 640px) {
 .measbar { grid-template-columns: repeat(3, 1fr); }
 .brand h1 { font-size: 1.05rem; }
 }
 </style>
</head>
<body>
 <header class="topbar">
 <div class="brand">
 <div class="logo">
 <svg viewBox="0 0 24 24" fill="none" stroke="#3dff9a" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
 <path d="M2 12h4l2-7 4 14 2-9 2 4h6"/>
 </svg>
 </div>
 <div>
 <h1>Osci<span>_ESP32</span> <small style="font-size:0.6rem; color:var(--accent);">8-bit Turbo</small></h1>
 <p class="sub">Digital Oscilloscope</p>
 </div>
 </div>
 <div class="status">
 <span id="led" class="led"></span>
 <span id="statusText">Conectando...</span>
 </div>
 </header>
 <main class="layout">
 <section class="screen-wrap">
 <div class="screen">
 <canvas id="scope"></canvas>
 </div>
 <div class="measbar">
 <div class="meas"><div class="k">Vpp</div><div class="v" id="mVpp">--</div></div>
 <div class="meas"><div class="k">Max</div><div class="v" id="mMax">--</div></div>
 <div class="meas"><div class="k">Min</div><div class="v" id="mMin">--</div></div>
 <div class="meas dac"><div class="k">DAC</div>
 <select id="dacFreq" title="Frequência do gerador de sinal (GPIO25)">
 <option value="122">122 Hz</option>
 <option value="400">400 Hz</option>
 <option value="1000">1 kHz</option>
 <option value="2000">2 kHz</option>
 <option value="5000" selected>5 kHz</option>
 <option value="10000">10 kHz</option>
 </select>
 </div>
 <div class="meas"><div class="k">Freq</div><div class="v" id="mFreq">--</div></div>
 <div class="meas battery"><div class="k">Bateria</div><div class="v" id="mBattery">--</div></div>
 <div class="meas trig"><div class="k">Trigger</div><div class="v" id="mTrig">OFF</div></div>
 </div>
 </section>
 <section class="panels">
 <div class="card">
 <h2>Vertical</h2>
 <div class="ctrl">
 <div class="ctrl-head"><label>Escala da chave</label><span class="val" id="vScaleVal">--</span></div>
 <p class="hint" style="margin-top:0;">Definida pela chave rotativa ligada ao ESP32.</p>
 </div>
 <div class="ctrl">
 <div class="ctrl-head"><label>Zoom</label><span class="val" id="vZoomVal">1.0x</span></div>
 <input type="range" id="vZoom" min="0.3" max="3" step="0.1" value="1">
 </div>
 <div class="ctrl">
 <div class="ctrl-head"><label>Offset</label><span class="val" id="vOffsetVal">0 px</span></div>
 <input type="range" id="vOffset" min="-100" max="100" step="1" value="0">
 </div>
 </div>
 <div class="card">
 <h2>Horizontal</h2>
 <div class="ctrl">
 <div class="ctrl-head"><label>Base de tempo</label><span class="val" id="hScaleDisplay">1 ms/div</span></div>
 <input type="range" id="hScale" min="0" max="8" step="1" value="4">
 </div>
 <p class="hint">Taxa de amostragem e tamanho da captura são ajustados dinamicamente.</p>
 </div>
 <div class="card">
 <h2>Trigger (Borda de Subida)</h2>
 <div class="ctrl switch-row">
 <label for="triggerEnable">Ativar Trigger</label>
 <label class="switch">
 <input type="checkbox" id="triggerEnable" checked>
 <span class="slider-sw"></span>
 </label>
 </div>
 <div class="ctrl">
 <div class="ctrl-head"><label>Modo</label></div>
 <select id="triggerMode">
 <option value="AUTO" selected>Auto (Com Timeout)</option>
 <option value="NORMAL">Normal (Espera Sync)</option>
 </select>
 </div>
 <div class="ctrl">
 <div class="ctrl-head"><label>Nível</label><span class="val" id="triggerLevelDisplay">128</span></div>
 <input type="range" id="triggerLevel" min="0" max="255" value="128">
 </div>
 <div class="ctrl">
 <div class="ctrl-head"><label>Histerese</label><span class="val" id="triggerHystDisplay">8</span></div>
 <input type="range" id="triggerHyst" min="1" max="30" step="1" value="8">
 </div>
 <p class="hint">Detecção no ESP32 em 12 bits · pré-trigger 15% · fallback no cliente marcado com *.</p>
 </div>
 <div class="card">
 <h2>Aquisição & Render</h2>
 <button id="btnStart" class="btn"><span class="dot"></span><span id="btnLabel">STOP</span></button>
 <div class="ctrl" style="margin-top: 12px;">
 <div class="ctrl-head"><label>Renderizador</label></div>
 <select id="renderMode">
 <option value="SPLINE" selected>Catmull-Rom (Spline Cúbica)</option>
 <option value="LINEAR">Linear (Padrão)</option>
 </select>
 </div>
 <p class="hint" id="info">Conectando...</p>
 </div>
 </section>
 </main>
 <script>
 (function() {
 const canvas = document.getElementById('scope');
 const ctx = canvas.getContext('2d');
 const info = document.getElementById('info');
 const led = document.getElementById('led');
 const statusText = document.getElementById('statusText');

 const hScaleSlider = document.getElementById('hScale');
 const hScaleDisplay = document.getElementById('hScaleDisplay');
 const vScaleVal = document.getElementById('vScaleVal');
 const vZoom = document.getElementById('vZoom');
 const vZoomVal = document.getElementById('vZoomVal');
 const vOffset = document.getElementById('vOffset');
 const vOffsetVal = document.getElementById('vOffsetVal');

 const btnStart = document.getElementById('btnStart');
 const btnLabel = document.getElementById('btnLabel');
 const triggerEnable = document.getElementById('triggerEnable');
 const triggerMode = document.getElementById('triggerMode');
 const triggerLevel = document.getElementById('triggerLevel');
 const triggerLevelDisplay = document.getElementById('triggerLevelDisplay');
 const triggerHyst = document.getElementById('triggerHyst');
 const triggerHystDisplay = document.getElementById('triggerHystDisplay');
 const renderMode = document.getElementById('renderMode');
 const dacFreq = document.getElementById('dacFreq');

 const mVpp = document.getElementById('mVpp');
 const mMax = document.getElementById('mMax');
 const mMin = document.getElementById('mMin');
 const mFreq = document.getElementById('mFreq');
 const mBattery = document.getElementById('mBattery');
 const mTrig = document.getElementById('mTrig');

 const HEADER_SIZE = 5;
 const DISPLAY_POINTS = 1024;
 const MAX_SAMPLE_RATE = 600000;
 const MAX_CAPTURE_SAMPLES = 4096;
 const TRIGGER_MARGIN = 512;
 const MAX_TRIGGERED_SAMPLES = MAX_CAPTURE_SAMPLES - TRIGGER_MARGIN;
 const VERTICAL_SCALE = [0, 0.001, 0.010, 0.100, 1.000, 10.000];

 const TIMEBASES = [
 { label: '100 ms/div', timePerDiv: 0.100 },
 { label: '50 ms/div', timePerDiv: 0.050 },
 { label: '10 ms/div', timePerDiv: 0.010 },
 { label: '5 ms/div', timePerDiv: 0.005 },
 { label: '1 ms/div', timePerDiv: 0.001 },
 { label: '500 µs/div', timePerDiv: 0.0005 },
 { label: '100 µs/div', timePerDiv: 0.0001 },
 { label: '50 µs/div', timePerDiv: 0.00005 },
 { label: '1 µs/div', timePerDiv: 0.000001 }
 ];

 const TARGET_FPS = 40;
 const MIN_FRAME_MS = 1000 / TARGET_FPS;
 let drawPending = false;
 let lastDrawTime = 0;

 let ws = null;
 let running = true;
 let latestFrame = new Uint8Array(0);
 let frameFilled = 0;
 let scalePosition = 1;
 let batteryRaw8 = 0;
 let requestedCaptureSamples = 0;
 let requestedSampleRate = 0;

 let serverEdgeFound = false;
 let serverEdgePos = -1;
 let serverEdgeFrac = 0;

 let W = 900, H = 360;
 let currentDpr = 1;
 let xPositions = new Float32Array(DISPLAY_POINTS);

 let smoothedFreq = 0;
 let freqValidCount = 0;
 let freqHistory = [];
 const MAX_FREQ_HISTORY = 10;

 let lastTriggered = null;
 let lastTriggeredSize = 0;
 let hasValidHold = false;
 let lastSyncTimestamp = 0;
 const AUTO_TIMEOUT_MS = 400;

 let plotWork = new Float32Array(DISPLAY_POINTS);

 function requestDraw() {
 if (drawPending) return;
 const elapsed = performance.now() - lastDrawTime;
 const wait = Math.max(0, MIN_FRAME_MS - elapsed);
 drawPending = true;
 setTimeout(() => {
 drawPending = false;
 lastDrawTime = performance.now();
 draw();
 }, wait);
 }

 function getCapturePlan() {
 const tb = TIMEBASES[parseInt(hScaleSlider.value)] || TIMEBASES[4];
 const screenTime = tb.timePerDiv * 10;
 const sampleRate = Math.min(MAX_SAMPLE_RATE, Math.max(1000, Math.floor(MAX_TRIGGERED_SAMPLES / screenTime)));
 const screenSamples = Math.max(16, Math.min(MAX_TRIGGERED_SAMPLES, Math.ceil(screenTime * sampleRate)));
 const captureSamples = Math.min(MAX_CAPTURE_SAMPLES, screenSamples + TRIGGER_MARGIN);
 return { screenSamples, captureSamples, sampleRate };
 }

 function sendCaptureSize() {
 const plan = getCapturePlan();
 if (ws && ws.readyState === WebSocket.OPEN && requestedCaptureSamples !== plan.captureSamples) {
 ws.send('cap:' + plan.captureSamples);
 requestedCaptureSamples = plan.captureSamples;
 }
 if (ws && ws.readyState === WebSocket.OPEN && requestedSampleRate !== plan.sampleRate) {
 ws.send('rate:' + plan.sampleRate);
 requestedSampleRate = plan.sampleRate;
 }
 }

 function sendTriggerConfig() {
 if (!ws || ws.readyState !== WebSocket.OPEN) return;
 const lv = parseInt(triggerLevel.value, 10) || 128;
 const hy = parseInt(triggerHyst.value, 10) || 8;
 const en = triggerEnable.checked ? 1 : 0;
 const plan = getCapturePlan();
 const pre = Math.max(8, Math.round(plan.screenSamples * 0.15));
 const post = Math.max(1, plan.screenSamples - pre);
 ws.send('trig:' + lv + ',' + hy + ',' + en + ',' + pre + ',' + post);
 }

 function resizeCanvas() {
 let dpr = window.devicePixelRatio || 1;
 if (window.innerWidth < 768) dpr = Math.min(dpr, 1.5);
 currentDpr = dpr;
 const cssW = canvas.clientWidth || 900;
 const cssH = Math.round(cssW * 0.42);
 W = cssW; H = cssH;
 canvas.style.height = cssH + 'px';
 canvas.width = Math.round(cssW * dpr);
 canvas.height = Math.round(cssH * dpr);
 ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
 for (let i = 0; i < DISPLAY_POINTS; i++) {
 xPositions[i] = (i / (DISPLAY_POINTS - 1)) * W;
 }
 draw();
 }
 window.addEventListener('resize', resizeCanvas);

 function paintRange(el) {
 const min = parseFloat(el.min), max = parseFloat(el.max);
 const pct = ((parseFloat(el.value) - min) / (max - min)) * 100;
 el.style.background = 'linear-gradient(90deg, var(--accent) ' + pct + '%, #16324a ' + pct + '%)';
 }
 [hScaleSlider, triggerLevel, triggerHyst, vZoom, vOffset].forEach(s => {
 paintRange(s);
 s.addEventListener('input', () => paintRange(s));
 });

 triggerLevel.addEventListener('input', () => {
 triggerLevelDisplay.textContent = triggerLevel.value;
 sendTriggerConfig();
 });
 triggerHyst.addEventListener('input', () => {
 triggerHystDisplay.textContent = triggerHyst.value;
 sendTriggerConfig();
 });
 triggerEnable.addEventListener('change', sendTriggerConfig);

 vZoom.addEventListener('input', () => { vZoomVal.textContent = parseFloat(vZoom.value).toFixed(1) + 'x'; });
 vOffset.addEventListener('input', () => { vOffsetVal.textContent = vOffset.value + ' px'; });

 hScaleSlider.addEventListener('input', () => {
 const tb = TIMEBASES[parseInt(hScaleSlider.value)] || TIMEBASES[4];
 hScaleDisplay.textContent = tb.label;
 resetFrequencyFilter();
 sendCaptureSize();
 sendTriggerConfig();
 });

 dacFreq.addEventListener('change', () => {
 if (ws && ws.readyState === WebSocket.OPEN) {
 ws.send('dac:' + dacFreq.value);
 }
 });

 function formatVerticalScale() {
 const voltsPerDivision = VERTICAL_SCALE[scalePosition];
 if (voltsPerDivision < 1) return (voltsPerDivision * 1000) + ' mV/div';
 return voltsPerDivision + ' V/div';
 }
 function updateVerticalScale() {
 vScaleVal.textContent = formatVerticalScale();
 }
 updateVerticalScale();

 (function() {
 const tb = TIMEBASES[parseInt(hScaleSlider.value)] || TIMEBASES[4];
 hScaleDisplay.textContent = tb.label;
 })();

 function setStatus(state, text) {
 led.className = 'led ' + state;
 statusText.textContent = text;
 }

 function connect() {
 if (ws) {
 ws.onopen = ws.onclose = ws.onerror = ws.onmessage = null;
 try { ws.close(); } catch(e) {}
 }

 const wsUrl = 'ws://' + window.location.hostname + ':81/';
 ws = new WebSocket(wsUrl);
 ws.binaryType = 'arraybuffer';

 ws.onopen = () => {
 setStatus('on', 'Conectado');
 info.innerText = 'Recebendo dados...';
 frameFilled = 0;
 lastTriggered = null;
 hasValidHold = false;
 serverEdgeFound = false;
 serverEdgePos = -1;
 serverEdgeFrac = 0;
 resetFrequencyFilter();
 sendCaptureSize();
 sendTriggerConfig();
 if (ws.readyState === WebSocket.OPEN) {
 ws.send('dac:' + dacFreq.value);
 ws.send('start');
 running = true;
 btnLabel.textContent = 'STOP';
 btnStart.classList.remove('stopped');
 }
 };

 ws.onclose = () => {
 setStatus('off', 'Desconectado');
 info.innerText = 'Reconectando...';
 setTimeout(connect, 1500);
 };

 ws.onerror = () => { setStatus('off', 'Erro de conexão'); };

 ws.onmessage = (evt) => {
 if (!running) return;
 const raw = new Uint8Array(evt.data);
 if (raw.length <= HEADER_SIZE) return;

 scalePosition = Math.max(1, Math.min(5, raw[0]));
 batteryRaw8 = raw[1];

 serverEdgeFound = (raw[3] & 0x80) !== 0;
 serverEdgePos = raw[2] | ((raw[3] & 0x0F) << 8);
 serverEdgeFrac = raw[4] / 256;

 mBattery.textContent = ((batteryRaw8 * 4.2) / 255).toFixed(2) + ' V';
 latestFrame = raw.slice(HEADER_SIZE);
 frameFilled = latestFrame.length;

 if (serverEdgeFound && (serverEdgePos < 0 || serverEdgePos >= frameFilled)) {
 serverEdgeFound = false;
 }

 updateVerticalScale();
 requestDraw();
 };
 }

 /* ============================================================
 TRIGGER — usa borda do ESP32 quando disponível;
 fallback cliente-side (borda MAIS RECENTE + interpolação).
 ============================================================ */
 const PRE_TRIGGER_RATIO = 0.15;

 function findBestRisingEdge(buf, total, level, hyst, postSamples, preSamples) {
 const armLevel = Math.max(0, level - Math.max(1, hyst));
 const minLow = Math.max(2, Math.min(8, (hyst | 0) + 1));
 const searchEnd = total - postSamples - 1;
 if (searchEnd < preSamples + 8) return -1;

 const searchStart = Math.max(preSamples + 1, 1);

 let lowCount = 0;
 let armed = false;
 let bestEdge = -1;
 let bestFrac = 0;

 for (let i = Math.max(0, searchStart - minLow); i < searchStart; i++) {
 if (buf[i] <= armLevel) lowCount++;
 else lowCount = 0;
 }
 armed = lowCount >= minLow;

 for (let i = searchStart; i <= searchEnd; i++) {
 const v = buf[i];
 if (v <= armLevel) {
 lowCount++;
 if (lowCount >= minLow) armed = true;
 } else {
 lowCount = 0;
 }

 if (armed && v >= level) {
 const prev = buf[i - 1];
 if (prev < level) {
 const denom = v - prev;
 let frac = denom > 0 ? (level - prev) / denom : 0;
 frac = Math.max(0, Math.min(1, frac));
 bestEdge = i - 1;
 bestFrac = frac;
 armed = false;
 lowCount = 0;
 }
 }
 }

 if (bestEdge < 0) return -1;
 return bestEdge + bestFrac;
 }

 function median(values) {
 if (!values.length) return 0;
 const sorted = [...values].sort((a, b) => a - b);
 const middle = Math.floor(sorted.length / 2);
 return sorted.length % 2 === 0
 ? (sorted[middle - 1] + sorted[middle]) / 2
 : sorted[middle];
 }

 function resetFrequencyFilter() {
 smoothedFreq = 0;
 freqValidCount = 0;
 freqHistory = [];
 mFreq.textContent = '--';
 }

 function getTriggeredData(level, hyst) {
 const plan = getCapturePlan();
 const need = plan.screenSamples;
 const avail = frameFilled;

 if (avail < need + 32) return null;

 const pre = Math.max(8, Math.round(need * PRE_TRIGGER_RATIO));
 const post = need - pre;

 // --- Caminho preferido: borda detectada no ESP32 ---
 if (serverEdgeFound &&
 serverEdgePos >= pre &&
 (serverEdgePos + post) <= avail) {
 const startIdx = serverEdgePos - pre;
 return {
 samples: latestFrame.subarray(startIdx, startIdx + need + 1),
 frac: serverEdgeFrac,
 source: 'server'
 };
 }

 // --- Fallback: detecção no cliente ---
 const edge = findBestRisingEdge(latestFrame, avail, level, hyst, post, pre);
 if (edge < 0) return null;

 const edgeIdx = Math.floor(edge);
 const edgeFrac = edge - edgeIdx;
 const startIdx = edgeIdx - pre;
 if (startIdx < 0 || startIdx + need > avail) return null;

 return {
 samples: latestFrame.subarray(startIdx, startIdx + need + 1),
 frac: edgeFrac,
 source: 'client'
 };
 }

 function updateMeasurements(plotData) {
 if (!plotData || plotData.length < 2) return;
 const vDiv = VERTICAL_SCALE[scalePosition];
 let mn = 255, mx = 0;
 for (let i = 0; i < plotData.length; i++) {
 const v = plotData[i];
 if (v < mn) mn = v;
 if (v > mx) mx = v;
 }
 const adcToVolts = adc => (adc - 127) * vDiv / 25;
 const formatVolts = volts => Math.abs(volts) < 1
 ? (volts * 1000).toFixed(2) + ' mV'
 : volts.toFixed(3) + ' V';
 mVpp.textContent = formatVolts(adcToVolts(mx) - adcToVolts(mn));
 mMax.textContent = formatVolts(adcToVolts(mx));
 mMin.textContent = formatVolts(adcToVolts(mn));
 }

 function measureFrequency(data, level, hyst) {
 if (!data || data.length < 16) {
 if (freqValidCount > 0) freqValidCount--;
 if (freqValidCount <= 0) resetFrequencyFilter();
 return;
 }

 const armLevel = Math.max(0, level - Math.max(1, hyst));
 const minLow = Math.max(2, Math.min(6, (hyst | 0)));
 const edges = [];
 let lowCount = 0;
 let armed = data[0] <= armLevel;

 for (let i = 1; i < data.length && edges.length < 48; i++) {
 const v = data[i];
 const prev = data[i - 1];
 if (v <= armLevel) {
 lowCount++;
 if (lowCount >= minLow) armed = true;
 } else {
 lowCount = 0;
 }
 if (armed && v >= level && prev < level) {
 edges.push(i);
 armed = false;
 lowCount = 0;
 }
 }

 if (edges.length < 2) {
 if (freqValidCount > 0) freqValidCount--;
 if (freqValidCount <= 0) resetFrequencyFilter();
 return;
 }

 const periods = [];
 for (let i = 1; i < edges.length; i++) periods.push(edges[i] - edges[i - 1]);
 const medianPeriod = median(periods);
 if (medianPeriod < 1) return;

 const lo = medianPeriod * 0.75, hi = medianPeriod * 1.25;
 let sumP = 0, countP = 0;
 for (let i = 0; i < periods.length; i++) {
 if (periods[i] >= lo && periods[i] <= hi) { sumP += periods[i]; countP++; }
 }
 if (countP < 1) return;

 const rawFreq = getCapturePlan().sampleRate / (sumP / countP);
 if (!Number.isFinite(rawFreq) || rawFreq <= 0) return;

 freqHistory.push(rawFreq);
 if (freqHistory.length > MAX_FREQ_HISTORY) freqHistory.shift();

 smoothedFreq = median(freqHistory);
 freqValidCount = Math.min(30, freqValidCount + 1);

 if (smoothedFreq >= 1000000) mFreq.textContent = (smoothedFreq / 1000000).toFixed(2) + ' MHz';
 else if (smoothedFreq >= 1000) mFreq.textContent = (smoothedFreq / 1000).toFixed(2) + ' kHz';
 else if (smoothedFreq >= 100) mFreq.textContent = smoothedFreq.toFixed(0) + ' Hz';
 else mFreq.textContent = smoothedFreq.toFixed(1) + ' Hz';
 }

 function drawGrid() {
 ctx.fillStyle = '#030507';
 ctx.fillRect(0, 0, W, H);
 const divX = W / 10, divY = H / 10;
 ctx.strokeStyle = 'rgba(29,78,110,0.35)';
 ctx.lineWidth = 1;
 ctx.beginPath();
 for (let x = divX; x < W; x += divX) { ctx.moveTo(x, 0); ctx.lineTo(x, H); }
 for (let y = divY; y < H; y += divY) { ctx.moveTo(0, y); ctx.lineTo(W, y); }
 ctx.stroke();
 ctx.strokeStyle = 'rgba(45,110,150,0.7)';
 ctx.beginPath();
 ctx.moveTo(0, H/2); ctx.lineTo(W, H/2);
 ctx.moveTo(W/2, 0); ctx.lineTo(W/2, H);
 ctx.stroke();
 ctx.strokeStyle = 'rgba(90,150,190,0.6)';
 ctx.beginPath();
 for (let x = 0; x <= W; x += divX / 5) { ctx.moveTo(x, H/2 - 4); ctx.lineTo(x, H/2 + 4); }
 for (let y = 0; y <= H; y += divY / 5) { ctx.moveTo(W/2 - 4, y); ctx.lineTo(W/2 + 4, y); }
 ctx.stroke();
 ctx.strokeStyle = 'rgba(45,110,150,0.5)';
 ctx.lineWidth = 1;
 ctx.strokeRect(0.5, 0.5, W-1, H-1);
 }

 function catmullRom(p0, p1, p2, p3, t) {
 const v0 = (p2 - p0) * 0.5;
 const v1 = (p3 - p1) * 0.5;
 const t2 = t * t;
 const t3 = t2 * t;
 return (2 * p1 - 2 * p2 + v0 + v1) * t3 +
 (-3 * p1 + 3 * p2 - 2 * v0 - v1) * t2 +
 v0 * t + p1;
 }

 function resampleToDisplay(src, srcLen, dest, startFrac, useSpline) {
 const fracOff = (typeof startFrac === 'number') ? startFrac : 0;
 const usable = Math.max(1, srcLen - fracOff);
 const step = (usable - 1) / (DISPLAY_POINTS - 1);

 for (let i = 0; i < DISPLAY_POINTS; i++) {
 const idx = fracOff + i * step;
 const idx1 = Math.floor(idx);
 const f = idx - idx1;

 if (useSpline) {
 const idx0 = Math.max(0, idx1 - 1);
 const idx2 = Math.min(srcLen - 1, idx1 + 1);
 const idx3 = Math.min(srcLen - 1, idx1 + 2);
 dest[i] = catmullRom(src[idx0], src[idx1], src[idx2], src[idx3], f);
 } else {
 const idx2 = Math.min(srcLen - 1, idx1 + 1);
 dest[i] = src[idx1] + (src[idx2] - src[idx1]) * f;
 }
 }
 }

 function draw() {
 ctx.setTransform(currentDpr, 0, 0, currentDpr, 0, 0);
 drawGrid();

 const trigActive = triggerEnable.checked;
 const isAuto = triggerMode.value === 'AUTO';
 const trigLevel = parseInt(triggerLevel.value);
 const trigHyst = parseInt(triggerHyst.value);
 const useSpline = renderMode.value === 'SPLINE';
 const zoom = parseFloat(vZoom.value);
 const offsetPx = parseInt(vOffset.value);

 const plan = getCapturePlan();

 let plotData = null;
 let trigFound = false;
 let trigSource = null;
 let fullWindowForFreq = null;
 let isAutoFallback = false;

 const now = performance.now();

 if (trigActive) {
 const triggered = getTriggeredData(trigLevel, trigHyst);

 if (triggered && triggered.samples) {
 lastTriggered = new Uint8Array(triggered.samples);
 lastTriggeredSize = triggered.samples.length;
 hasValidHold = true;
 lastSyncTimestamp = now;

 fullWindowForFreq = triggered.samples;
 resampleToDisplay(triggered.samples, triggered.samples.length, plotWork, triggered.frac, useSpline);
 plotData = plotWork;
 trigFound = true;
 trigSource = triggered.source;
 } else if (isAuto && (now - lastSyncTimestamp > AUTO_TIMEOUT_MS)) {
 isAutoFallback = true;
 if (frameFilled >= plan.screenSamples) {
 const dataSegment = latestFrame.subarray(0, plan.screenSamples);
 fullWindowForFreq = dataSegment;
 resampleToDisplay(dataSegment, dataSegment.length, plotWork, 0, useSpline);
 plotData = plotWork;
 }
 } else if (hasValidHold && lastTriggered && lastTriggeredSize >= 16) {
 fullWindowForFreq = lastTriggered;
 resampleToDisplay(lastTriggered, lastTriggeredSize, plotWork, 0, useSpline);
 plotData = plotWork;
 trigSource = 'hold';
 }
 } else {
 hasValidHold = false;
 lastTriggered = null;
 if (frameFilled >= plan.screenSamples) {
 const dataSegment = latestFrame.subarray(0, plan.screenSamples);
 fullWindowForFreq = dataSegment;
 resampleToDisplay(dataSegment, dataSegment.length, plotWork, 0, useSpline);
 plotData = plotWork;
 }
 }

 if (fullWindowForFreq) {
 measureFrequency(fullWindowForFreq, trigLevel, trigHyst);
 }

 if (trigActive) {
 const halfH = H / 2;
 const baseY = (H / 10) / 25;
 const scaleY = baseY * zoom;
 const yTrigger = halfH - (trigLevel - 127.5) * scaleY + offsetPx;

 ctx.strokeStyle = '#ffb454';
 ctx.lineWidth = 1.2;
 ctx.setLineDash([6, 5]);
 ctx.beginPath(); ctx.moveTo(0, yTrigger); ctx.lineTo(W, yTrigger); ctx.stroke();
 ctx.setLineDash([]);

 const preX = W * PRE_TRIGGER_RATIO;
 ctx.beginPath();
 ctx.moveTo(preX, 0);
 ctx.lineTo(preX, H);
 ctx.strokeStyle = 'rgba(255,180,84,0.35)';
 ctx.setLineDash([3, 4]);
 ctx.stroke();
 ctx.setLineDash([]);

 ctx.fillStyle = '#ffb454';
 ctx.font = '11px Consolas, monospace';
 ctx.fillText('T ' + trigLevel, 6, Math.max(12, yTrigger - 5));
 }

 updateTrigBadge(trigActive, trigFound, isAutoFallback, trigSource);

 if (plotData && plotData.length >= 2) {
 ctx.strokeStyle = '#3dff9a';
 ctx.lineWidth = 1.8;
 ctx.lineJoin = 'round';
 ctx.beginPath();
 const halfH = H / 2;
 const baseY = (H / 10) / 25;
 const scaleY = baseY * zoom;
 for (let i = 0; i < plotData.length; i++) {
 const x = xPositions[i];
 let y = halfH - (plotData[i] - 127.5) * scaleY + offsetPx;
 y = y < 0 ? 0 : (y > H ? H : y);
 if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
 }
 ctx.stroke();
 updateMeasurements(plotData);
 }
 }

 function updateTrigBadge(active, found, isAutoFallback, source) {
 if (!active) {
 mTrig.textContent = 'OFF';
 mTrig.style.color = 'var(--text-dim)';
 return;
 }
 if (found) {
 if (source === 'server') {
 mTrig.textContent = 'SYNC';
 mTrig.style.color = 'var(--trace)';
 } else if (source === 'client') {
 mTrig.textContent = 'SYNC*';
 mTrig.style.color = 'var(--accent)';
 } else {
 mTrig.textContent = 'HOLD';
 mTrig.style.color = 'var(--trig)';
 }
 } else if (isAutoFallback) {
 mTrig.textContent = 'AUTO';
 mTrig.style.color = 'var(--accent)';
 } else {
 mTrig.textContent = 'WAIT';
 mTrig.style.color = 'var(--trig)';
 }
 }

 [hScaleSlider, triggerLevel, triggerHyst, triggerEnable, triggerMode, renderMode, vZoom, vOffset].forEach(el =>
 el.addEventListener('input', draw)
 );

 btnStart.onclick = () => {
 if (ws && ws.readyState === WebSocket.OPEN) {
 if (running) {
 ws.send('stop');
 info.innerText = 'Transmissão pausada';
 } else {
 ws.send('start');
 info.innerText = 'Recebendo dados...';
 }
 }
 running = !running;
 if (running) { btnLabel.textContent = 'STOP'; btnStart.classList.remove('stopped'); }
 else { btnLabel.textContent = 'RUN'; btnStart.classList.add('stopped'); }
 };

 resizeCanvas();
 connect();
 })();
 </script>
</body>
</html>
)rawliteral";

#endif