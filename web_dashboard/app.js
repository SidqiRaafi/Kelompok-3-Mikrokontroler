"use strict";

// ── Konfigurasi ──────────────────────────────────────────────────────────────
const BROKER_URL =
  "wss://a2b717741b664ec883fd2bf66d9eb680.s1.eu.hivemq.cloud:8884/mqtt";
const TOPIC = "gasleak/data";
const THRESHOLD = 3000;
const ADC_MAX = 4095;
const MAX_HIST = 30;
const STALE_MS = 10000;

// r=110, busur 270°: circ = 2π×110 = 691.15, arc = 691.15 × 0.75 = 518.36
const CIRC = 2 * Math.PI * 110;
const ARC_LEN = CIRC * (270 / 360);

const mqttOptions = {
  username: "gass12",
  password: "GasBocor123",
  reconnectPeriod: 1500,
  keepalive: 30,
  connectTimeout: 10000,
  clean: true,
};

// ── DOM refs ─────────────────────────────────────────────────────────────────
const $ = (id) => document.getElementById(id);

// Indikator koneksi MQTT
const connLed = $("conn-led");
const connText = $("conn-text");

// LED kebocoran
const leakLed = $("leak-led");

const gasEl = $("gas-value");
const maxEl = $("max-value");
const minEl = $("min-value");
const gaugeArc = $("gauge-arc");
const gaugeNum = $("gauge-num");
const canvas = $("sparkline");
const lastUpdEl = $("last-update-text");
const freshDot = $("freshness-dot");
const ctx = canvas.getContext("2d");

// ── State ─────────────────────────────────────────────────────────────────────
const history = [];
let maxSeen = null;
let minSeen = null;
let staleTimer = null;
let chartW = 0;
const CHART_H = 80;

// ── Canvas ────────────────────────────────────────────────────────────────────
function initCanvas() {
  const dpr = window.devicePixelRatio || 1;
  const rect = canvas.getBoundingClientRect();
  chartW = Math.round(rect.width) || canvas.parentElement.clientWidth - 56;
  canvas.width = chartW * dpr;
  canvas.height = CHART_H * dpr;
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
}

window.addEventListener("load", () =>
  requestAnimationFrame(() => {
    initCanvas();
    drawSparkline();
  }),
);
window.addEventListener("resize", () => {
  initCanvas();
  drawSparkline();
});

// ── Gauge ─────────────────────────────────────────────────────────────────────
function updateGauge(value) {
  const v = Math.max(0, Math.min(Number(value), ADC_MAX));
  const filled = (v / ADC_MAX) * ARC_LEN;
  const gap = CIRC - filled;

  gaugeArc.setAttribute(
    "stroke-dasharray",
    `${filled.toFixed(2)} ${gap.toFixed(2)}`,
  );

  // Arc: Soft Green (aman) → Beige (peringatan) → Merah (bahaya)
  const arcColor =
    v >= THRESHOLD
      ? "#c0392b" // Merah — bahaya
      : v >= Math.round(THRESHOLD * 0.65)
        ? "#C9AD93" // Beige — peringatan
        : "#879A77"; // Soft Green — aman

  // Angka: Hitam pada kondisi normal, Merah saat bahaya
  const numColor = v >= THRESHOLD ? "#c0392b" : "#000000";

  gaugeArc.style.stroke = arcColor;
  gaugeNum.style.fill = numColor;
  gaugeNum.textContent = String(v);
}

// ── Sparkline ─────────────────────────────────────────────────────────────────
function drawSparkline() {
  if (!chartW || history.length < 2) return;
  ctx.clearRect(0, 0, chartW, CHART_H);

  const pts = history;
  const n = pts.length;
  const peak = Math.max(ADC_MAX * 0.5, ...pts);
  const pad = { t: 10, b: 10 };
  const iH = CHART_H - pad.t - pad.b;

  const scaleY = (v) => pad.t + (1 - v / peak) * iH;
  const step = chartW / (MAX_HIST - 1);
  const xOf = (i) => i * step;

  // Garis threshold — Beige putus-putus
  const ty = scaleY(THRESHOLD);
  ctx.save();
  ctx.setLineDash([4, 5]);
  ctx.strokeStyle = "rgba(201,173,147,0.65)";
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(0, ty);
  ctx.lineTo(chartW, ty);
  ctx.stroke();
  ctx.restore();

  const last = pts[n - 1];
  const isWarn = last >= Math.round(THRESHOLD * 0.65);
  const isDanger = last >= THRESHOLD;

  // Warna garis: Hitam (aman) → Taupe (peringatan) → Merah (bahaya)
  const lineRGB = isDanger ? "192,57,43" : isWarn ? "85,73,64" : "0,0,0";

  // Area fill gradasi
  const grad = ctx.createLinearGradient(0, pad.t, 0, CHART_H);
  grad.addColorStop(0, `rgba(${lineRGB},0.07)`);
  grad.addColorStop(1, `rgba(${lineRGB},0)`);
  ctx.beginPath();
  pts.forEach((v, i) =>
    i === 0 ? ctx.moveTo(xOf(i), scaleY(v)) : ctx.lineTo(xOf(i), scaleY(v)),
  );
  ctx.lineTo(xOf(n - 1), CHART_H);
  ctx.lineTo(0, CHART_H);
  ctx.closePath();
  ctx.fillStyle = grad;
  ctx.fill();

  // Garis utama
  ctx.beginPath();
  pts.forEach((v, i) =>
    i === 0 ? ctx.moveTo(xOf(i), scaleY(v)) : ctx.lineTo(xOf(i), scaleY(v)),
  );
  ctx.strokeStyle = `rgb(${lineRGB})`;
  ctx.lineWidth = 1.2;
  ctx.lineJoin = "round";
  ctx.lineCap = "round";
  ctx.stroke();

  // Titik data terkini
  ctx.beginPath();
  ctx.arc(xOf(n - 1), scaleY(last), 3.5, 0, Math.PI * 2);
  ctx.fillStyle = `rgb(${lineRGB})`;
  ctx.fill();
}

// ── Freshness ─────────────────────────────────────────────────────────────────
function markFresh() {
  clearTimeout(staleTimer);
  freshDot.className = "freshness-dot fresh";
  staleTimer = setTimeout(() => {
    freshDot.className = "freshness-dot";
    lastUpdEl.textContent = "Data tidak masuk (>10 detik) — cek koneksi ESP32";
  }, STALE_MS);
}

function formatTime(d) {
  return d.toLocaleTimeString("id-ID", {
    hour: "2-digit",
    minute: "2-digit",
    second: "2-digit",
  });
}

// ── Handler data ───────────────────────────────────────
function onGasValue(value) {
  history.push(value);
  if (history.length > MAX_HIST) history.shift();

  if (maxSeen === null || value > maxSeen) {
    maxSeen = value;
    maxEl.textContent = value;
  }
  if (minSeen === null || value < minSeen) {
    minSeen = value;
    minEl.textContent = value;
  }

  gasEl.textContent = value;
  updateGauge(value);
  drawSparkline();

  // LED kebocoran: merah menyala+berkedip saat bocor, mati saat aman
  const isLeak = value >= THRESHOLD;
  leakLed.className = `led-dot led-dot--lg${isLeak ? " led-on-red" : ""}`;

  lastUpdEl.textContent = `Diperbarui pukul ${formatTime(new Date())}`;
  markFresh();
}

// ── MQTT ──────────────────────────────────────────────────────────────────────
const client = mqtt.connect(BROKER_URL, mqttOptions);

client.on("connect", () => {
  console.log("[MQTT] Terhubung ke HiveMQ Cloud");
  connLed.className = "led-dot led-on-green";
  connText.textContent = "Terhubung";
  client.subscribe(TOPIC, (err) => {
    if (err) console.error("[MQTT] Gagal subscribe:", err);
    else console.log(`[MQTT] Subscribed ke: ${TOPIC}`);
  });
});

client.on("message", (_topic, message) => {
  const raw = message.toString();
  console.log("[MQTT] Data masuk:", raw);
  try {
    const data = JSON.parse(raw);
    const value = Number(data.raw ?? data.gas_value ?? 0);
    if (Number.isFinite(value)) onGasValue(value);
    else console.warn("[MQTT] Nilai gas tidak valid:", data);
  } catch {
    const value = Number(raw);
    if (Number.isFinite(value)) onGasValue(value);
    else console.warn("[MQTT] Format tidak dikenali:", raw);
  }
});

client.on("reconnect", () => {
  console.log("[MQTT] Mencoba reconnect...");
  connLed.className = "led-dot"; // LED mati
  connText.textContent = "Menghubungkan\u2026";
});

client.on("close", () => {
  console.log("[MQTT] Koneksi tertutup");
  connLed.className = "led-dot"; // LED mati
  connText.textContent = "Terputus";
});

client.on("error", (err) => {
  console.error("[MQTT] Error:", err);
  connLed.className = "led-dot";
  connText.textContent = "Error";
});

client.on("offline", () => {
  connLed.className = "led-dot";
  connText.textContent = "Offline";
});
