#include "httplib.h"
#include "Queue.h"
#include "Stack.h"
#include <iostream>
#include <string>
using namespace std;

Queue antrean;
Stack riwayat;

string ambilNilai(const string& json, const string& key) {
    string cari = "\"" + key + "\":\"";
    int pos = json.find(cari);
    if (pos == -1) return "";
    pos += cari.size();
    int akhir = json.find("\"", pos);
    if (akhir == -1) return "";
    return json.substr(pos, akhir - pos);
}

string nodeKeJson(Node* node) {
    return "{\"nama\":\""   + node->data.nama    + "\","
            "\"nim\":\""    + node->data.nim     + "\","
            "\"judul\":\""  + node->data.judulTA + "\","
            "\"status\":\"" + node->data.status  + "\"}";
}

string buatResponse() {
    string qJson = "[";
    Node* curr = antrean.getHead();
    bool pertama = true;
    while (curr != nullptr) {
        if (!pertama) qJson += ",";
        qJson += nodeKeJson(curr);
        pertama = false;
        curr = curr->next;
    }
    qJson += "]";

    string sJson = "[";
    curr = riwayat.getTop();
    pertama = true;
    while (curr != nullptr) {
        if (!pertama) sJson += ",";
        sJson += nodeKeJson(curr);
        pertama = false;
        curr = curr->next;
    }
    sJson += "]";

    return "{\"queue\":"     + qJson +
           ",\"stack\":"     + sJson +
           ",\"queueSize\":" + to_string(antrean.getSize()) +
           ",\"stackSize\":" + to_string(riwayat.getSize()) + "}";
}

const string HALAMAN = R"html(<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8"/>
<meta name="viewport" content="width=device-width, initial-scale=1.0"/>
<title>Sistem Antrean Sidang TA</title>
<link href="https://fonts.googleapis.com/css2?family=DM+Mono:wght@400;500&family=Instrument+Serif:ital@0;1&family=DM+Sans:wght@300;400;500&display=swap" rel="stylesheet"/>
<style>
  *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
  :root {
    --bg:#0f0f0f;--bg2:#161616;--bg3:#1e1e1e;--border:#2a2a2a;--border2:#383838;
    --text:#e8e6e0;--muted:#777;--muted2:#4a4a4a;--accent:#c8b560;--accent2:#8a7a35;
    --green:#4caf78;--red:#e05a5a;--blue:#5a9be0;
    --mono:'DM Mono',monospace;--serif:'Instrument Serif',serif;--sans:'DM Sans',sans-serif;--r:6px;
  }
  body { background:var(--bg);color:var(--text);font-family:var(--sans);font-size:14px;min-height:100vh;line-height:1.6; }
  .shell { display:grid;grid-template-columns:260px 1fr 300px;grid-template-rows:56px 1fr;min-height:100vh; }
  header { grid-column:1/-1;background:var(--bg2);border-bottom:1px solid var(--border);display:flex;align-items:center;padding:0 24px;gap:16px; }
  .logo-badge { font-family:var(--mono);font-size:11px;background:var(--accent);color:#0f0e08;padding:3px 8px;border-radius:3px;font-weight:500;letter-spacing:.04em;text-transform:uppercase; }
  .logo-title { font-family:var(--serif);font-size:17px;color:var(--text);font-style:italic; }
  .header-stats { margin-left:auto;display:flex;gap:24px;font-family:var(--mono);font-size:12px;color:var(--muted); }
  .header-stats span b { color:var(--text);font-weight:500; }
  .sidebar-left { background:var(--bg2);border-right:1px solid var(--border);padding:20px 16px;display:flex;flex-direction:column;gap:20px;overflow-y:auto; }
  .section-label { font-family:var(--mono);font-size:10px;letter-spacing:.12em;text-transform:uppercase;color:var(--muted);margin-bottom:10px; }
  .form-group { display:flex;flex-direction:column;gap:5px; }
  .form-group label { font-size:11px;color:var(--muted);font-family:var(--mono);letter-spacing:.04em; }
  .form-group input,.form-group textarea { background:var(--bg3);border:1px solid var(--border);color:var(--text);padding:8px 10px;border-radius:var(--r);font-family:var(--sans);font-size:13px;outline:none;transition:border-color .15s;resize:none; }
  .form-group input:focus,.form-group textarea:focus { border-color:var(--accent); }
  .form-group textarea { height:60px; }
  .btn { display:flex;align-items:center;justify-content:center;gap:7px;padding:9px 14px;border-radius:var(--r);border:none;cursor:pointer;font-family:var(--sans);font-size:13px;font-weight:500;transition:all .15s;width:100%; }
  .btn-primary { background:var(--accent);color:#0f0e08; }
  .btn-primary:hover { background:#d9c468; }
  .btn-ghost { background:transparent;border:1px solid var(--border2);color:var(--muted); }
  .btn-ghost:hover { border-color:var(--accent);color:var(--accent); }
  main { padding:20px;overflow-y:auto;display:flex;flex-direction:column;gap:16px; }
  .panel { background:var(--bg2);border:1px solid var(--border);border-radius:8px;overflow:hidden; }
  .panel-header { padding:12px 16px;border-bottom:1px solid var(--border);display:flex;align-items:center;justify-content:space-between; }
  .panel-title { font-family:var(--mono);font-size:11px;letter-spacing:.1em;text-transform:uppercase;color:var(--muted);display:flex;align-items:center;gap:8px; }
  .panel-title .dot { width:6px;height:6px;border-radius:50%;background:var(--accent);display:inline-block; }
  .count-badge { font-family:var(--mono);font-size:11px;background:var(--bg3);border:1px solid var(--border);color:var(--muted);padding:2px 8px;border-radius:20px; }
  .panel-body { padding:12px 0; }
  .queue-item { display:grid;grid-template-columns:36px 1fr auto;align-items:start;gap:10px;padding:10px 16px;border-bottom:1px solid var(--border);transition:background .1s;animation:slideIn .25s ease; }
  .queue-item:last-child { border-bottom:none; }
  .queue-item:hover { background:var(--bg3); }
  @keyframes slideIn { from{opacity:0;transform:translateY(-6px)} to{opacity:1;transform:translateY(0)} }
  .queue-num { font-family:var(--mono);font-size:18px;color:var(--muted2);font-weight:500;line-height:1.4;text-align:center; }
  .queue-num.first { color:var(--accent); }
  .queue-name { font-size:14px;font-weight:500;color:var(--text);margin-bottom:2px; }
  .queue-nim { font-family:var(--mono);font-size:11px;color:var(--muted);margin-bottom:4px; }
  .queue-judul { font-size:12px;color:var(--muted);line-height:1.4; }
  .status-tag { font-family:var(--mono);font-size:10px;letter-spacing:.05em;padding:3px 8px;border-radius:3px;text-transform:uppercase;white-space:nowrap; }
  .status-menunggu { background:#1a1608;color:var(--accent);border:1px solid var(--accent2); }
  .status-verified  { background:#0d1f14;color:var(--green);border:1px solid #2a6644; }
  .status-ditolak   { background:#1f0d0d;color:var(--red);border:1px solid #663030; }
  .empty-state { padding:32px 16px;text-align:center;color:var(--muted2);font-family:var(--mono);font-size:12px; }
  .empty-state .big { font-size:28px;margin-bottom:8px; }
  .sidebar-right { background:var(--bg2);border-left:1px solid var(--border);padding:20px 16px;display:flex;flex-direction:column;gap:20px;overflow-y:auto; }
  .verif-card { background:var(--bg3);border:1px solid var(--border);border-radius:8px;padding:14px; }
  .verif-card.highlight { border-color:var(--accent); }
  .verif-name { font-size:15px;font-weight:500;margin-bottom:3px; }
  .verif-nim  { font-family:var(--mono);font-size:11px;color:var(--muted);margin-bottom:6px; }
  .verif-judul { font-size:12px;color:var(--muted);line-height:1.5;margin-bottom:12px; }
  .verif-actions { display:flex;gap:8px; }
  .btn-verify { flex:1;padding:8px 10px;border-radius:var(--r);border:none;cursor:pointer;font-family:var(--mono);font-size:11px;font-weight:500;text-transform:uppercase;letter-spacing:.05em;transition:all .15s; }
  .btn-verify:active { transform:scale(.97); }
  .btn-approve { background:#0d1f14;color:var(--green);border:1px solid #2a6644; }
  .btn-approve:hover { background:#132b1c; }
  .btn-reject  { background:#1f0d0d;color:var(--red);border:1px solid #663030; }
  .btn-reject:hover { background:#2b1212; }
  .peek-card { background:var(--bg3);border:1px solid var(--border2);border-radius:8px;padding:14px; }
  .peek-label { font-family:var(--mono);font-size:10px;letter-spacing:.1em;text-transform:uppercase;color:var(--muted);margin-bottom:10px;display:flex;align-items:center;gap:6px; }
  .peek-label::before { content:'';width:16px;height:1px;background:var(--muted2); }
  .log-line { font-family:var(--mono);font-size:11px;color:var(--muted);padding:4px 0;border-bottom:1px solid var(--border);display:flex;align-items:center;gap:8px; }
  .log-line:last-child { border-bottom:none; }
  .log-line .ts { color:var(--muted2); }
  .log-line .ok { color:var(--green); } .log-line .err { color:var(--red); } .log-line .inf { color:var(--blue); }
  #toast-container { position:fixed;bottom:20px;right:20px;display:flex;flex-direction:column;gap:8px;z-index:999; }
  .toast { background:var(--bg3);border:1px solid var(--border2);color:var(--text);padding:10px 14px;border-radius:var(--r);font-size:13px;font-family:var(--mono);animation:toastIn .2s ease;max-width:280px; }
  @keyframes toastIn { from{opacity:0;transform:translateX(10px)} to{opacity:1;transform:translateX(0)} }
  .toast.success { border-left:3px solid var(--green); }
  .toast.error   { border-left:3px solid var(--red); }
  .toast.info    { border-left:3px solid var(--blue); }
  ::-webkit-scrollbar { width:4px; }
  ::-webkit-scrollbar-track { background:transparent; }
  ::-webkit-scrollbar-thumb { background:var(--border2);border-radius:2px; }
</style>
</head>
<body>
<div class="shell">
  <header>
    <span class="logo-badge">UGM</span>
    <span class="logo-title">Sistem Antrean Sidang Tugas Akhir</span>
    <div class="header-stats">
      <span>Antrean: <b id="h-queue">0</b></span>
      <span>Terverifikasi: <b id="h-verified">0</b></span>
      <span>Ditolak: <b id="h-rejected">0</b></span>
    </div>
  </header>
  <aside class="sidebar-left">
    <div>
      <div class="section-label">Tambah Pendaftar</div>
      <div style="display:flex;flex-direction:column;gap:10px;">
        <div class="form-group"><label>Nama Lengkap</label><input id="f-nama" type="text" placeholder="cth. Andi Saputra"/></div>
        <div class="form-group"><label>NIM</label><input id="f-nim" type="text" placeholder="cth. 22/123456/TK/12345"/></div>
        <div class="form-group"><label>Judul Tugas Akhir</label><textarea id="f-judul" placeholder="cth. Implementasi CNN untuk..."></textarea></div>
        <button class="btn btn-primary" onclick="tambahPendaftar()">+ Daftarkan ke Antrean</button>
      </div>
    </div>
    <div>
      <div class="section-label">Demo Data</div>
      <div style="display:flex;flex-direction:column;gap:6px;">
        <button class="btn btn-ghost" onclick="fillDemo(0)">Isi Contoh #1</button>
        <button class="btn btn-ghost" onclick="fillDemo(1)">Isi Contoh #2</button>
        <button class="btn btn-ghost" onclick="fillDemo(2)">Isi Contoh #3</button>
      </div>
    </div>
    <div style="flex:1;">
      <div class="section-label">Log Aktivitas</div>
      <div id="log-container"></div>
    </div>
  </aside>
  <main>
    <div class="panel">
      <div class="panel-header">
        <span class="panel-title"><span class="dot"></span>Antrean Pendaftaran (Queue - FIFO)</span>
        <span class="count-badge" id="q-count">0 pendaftar</span>
      </div>
      <div class="panel-body" id="queue-list"><div class="empty-state"><div class="big">-</div>Antrean kosong</div></div>
    </div>
    <div class="panel">
      <div class="panel-header">
        <span class="panel-title"><span class="dot" style="background:var(--blue)"></span>Riwayat Verifikasi (Stack - LIFO)</span>
        <span class="count-badge" id="s-count">0 data</span>
      </div>
      <div class="panel-body" id="stack-list"><div class="empty-state"><div class="big">-</div>Belum ada yang diverifikasi</div></div>
    </div>
  </main>
  <aside class="sidebar-right">
    <div>
      <div class="section-label">Proses Verifikasi Berikutnya</div>
      <div id="verif-area"><div class="verif-card" style="color:var(--muted2);font-family:var(--mono);font-size:12px;text-align:center;padding:24px 14px;">Tidak ada pendaftar</div></div>
    </div>
    <div>
      <div class="section-label">Pendaftar Terakhir Diproses (Peek)</div>
      <div id="peek-area"><div class="peek-card" style="color:var(--muted2);font-family:var(--mono);font-size:12px;text-align:center;padding:20px 14px;">Stack kosong</div></div>
    </div>
  </aside>
</div>
<div id="toast-container"></div>
<script>
const DEMO = [
  {nama:"Andi Saputra", nim:"22/123456/TK/12345", judul:"Implementasi CNN untuk Deteksi Penyakit Tanaman"},
  {nama:"Budi Santoso",  nim:"22/123457/TK/12346", judul:"Sistem Rekomendasi Film Berbasis Collaborative Filtering"},
  {nama:"Citra Dewi",    nim:"22/123458/TK/12347", judul:"Analisis Sentimen Media Sosial Menggunakan BERT"},
];

async function kirimRequest(url, data) {
  const opts = data
    ? {method:"POST", headers:{"Content-Type":"application/json"}, body:JSON.stringify(data)}
    : {method:"GET"};
  const res = await fetch(url, opts);
  if (!res.ok) throw new Error(await res.text());
  return res.json();
}

async function tambahPendaftar() {
  const nama  = document.getElementById("f-nama").value.trim();
  const nim   = document.getElementById("f-nim").value.trim();
  const judul = document.getElementById("f-judul").value.trim();
  if (!nama || !nim || !judul) { tampilToast("Harap isi semua field.", "error"); return; }
  try {
    const state = await kirimRequest("/enqueue", {nama, nim, judul});
    document.getElementById("f-nama").value  = "";
    document.getElementById("f-nim").value   = "";
    document.getElementById("f-judul").value = "";
    tambahLog("inf", "enqueue: " + nama);
    tampilToast(nama + " berhasil didaftarkan.", "success");
    render(state);
  } catch(e) { tampilToast("Error: " + e.message, "error"); }
}

async function verifikasi(approve) {
  try {
    const state = await kirimRequest("/verify", {approve});
    const label = approve ? "Terverifikasi" : "Ditolak";
    tambahLog(approve ? "ok" : "err", "verified -> " + label);
    tampilToast("Verifikasi: " + label, approve ? "success" : "error");
    render(state);
  } catch(e) { tampilToast("Error: " + e.message, "error"); }
}

async function muatState() {
  try { render(await kirimRequest("/state")); } catch(e) { console.error(e); }
}

function fillDemo(i) {
  document.getElementById("f-nama").value  = DEMO[i].nama;
  document.getElementById("f-nim").value   = DEMO[i].nim;
  document.getElementById("f-judul").value = DEMO[i].judul;
}

function render(state) {
  tampilQueue(state.queue);
  tampilStack(state.stack);
  tampilVerif(state.queue);
  tampilPeek(state.stack);
  document.getElementById("h-queue").textContent    = state.queueSize;
  document.getElementById("h-verified").textContent = state.stack.filter(m => m.status === "Terverifikasi").length;
  document.getElementById("h-rejected").textContent = state.stack.filter(m => m.status === "Ditolak").length;
  document.getElementById("q-count").textContent    = state.queueSize + " pendaftar";
  document.getElementById("s-count").textContent    = state.stackSize + " data";
}

function tampilQueue(q) {
  const el = document.getElementById("queue-list");
  if (!q.length) { el.innerHTML = '<div class="empty-state"><div class="big">-</div>Antrean kosong</div>'; return; }
  el.innerHTML = q.map((m, i) => `
    <div class="queue-item">
      <div class="queue-num ${i===0?"first":""}">${String(i+1).padStart(2,"0")}</div>
      <div>
        <div class="queue-name">${m.nama}</div>
        <div class="queue-nim">${m.nim}</div>
        <div class="queue-judul">${m.judul}</div>
      </div>
      <span class="status-tag status-menunggu">Menunggu</span>
    </div>`).join("");
}

function tampilStack(s) {
  const el = document.getElementById("stack-list");
  if (!s.length) { el.innerHTML = '<div class="empty-state"><div class="big">-</div>Belum ada yang diverifikasi</div>'; return; }
  el.innerHTML = s.map((m, i) => `
    <div class="queue-item">
      <div class="queue-num ${i===0?"first":""}">${String(i+1).padStart(2,"0")}</div>
      <div>
        <div class="queue-name">${m.nama}</div>
        <div class="queue-nim">${m.nim}</div>
        <div class="queue-judul">${m.judul}</div>
      </div>
      <span class="status-tag ${m.status==="Terverifikasi"?"status-verified":"status-ditolak"}">${m.status}</span>
    </div>`).join("");
}

function tampilVerif(q) {
  const el = document.getElementById("verif-area");
  if (!q.length) { el.innerHTML = '<div class="verif-card" style="color:var(--muted2);font-family:var(--mono);font-size:12px;text-align:center;padding:24px 14px;">Tidak ada pendaftar</div>'; return; }
  const n = q[0];
  el.innerHTML = `<div class="verif-card highlight">
    <div class="verif-name">${n.nama}</div>
    <div class="verif-nim">${n.nim}</div>
    <div class="verif-judul">${n.judul}</div>
    <div class="verif-actions">
      <button class="btn-verify btn-approve" onclick="verifikasi(true)">v Terverifikasi</button>
      <button class="btn-verify btn-reject"  onclick="verifikasi(false)">x Ditolak</button>
    </div></div>`;
}

function tampilPeek(s) {
  const el = document.getElementById("peek-area");
  if (!s.length) { el.innerHTML = '<div class="peek-card" style="color:var(--muted2);font-family:var(--mono);font-size:12px;text-align:center;padding:20px 14px;">Stack kosong</div>'; return; }
  const t = s[0];
  el.innerHTML = `<div class="peek-card">
    <div class="peek-label">top of stack</div>
    <div class="queue-name" style="margin-bottom:3px">${t.nama}</div>
    <div class="queue-nim"  style="margin-bottom:5px">${t.nim}</div>
    <div class="queue-judul" style="margin-bottom:10px">${t.judul}</div>
    <span class="status-tag ${t.status==="Terverifikasi"?"status-verified":"status-ditolak"}">${t.status}</span>
  </div>`;
}

function tambahLog(type, msg) {
  const c = document.getElementById("log-container");
  const ts = new Date().toTimeString().slice(0,8);
  const sym = {ok:"v", err:"x", inf:">"}[type] || "-";
  const d = document.createElement("div");
  d.className = "log-line";
  d.innerHTML = `<span class="ts">${ts}</span><span class="${type}">${sym}</span><span>${msg}</span>`;
  c.prepend(d);
  while (c.children.length > 30) c.removeChild(c.lastChild);
}

function tampilToast(msg, type) {
  const c = document.getElementById("toast-container");
  const t = document.createElement("div");
  t.className = "toast " + (type || "info");
  t.textContent = msg;
  c.appendChild(t);
  setTimeout(() => t.remove(), 3000);
}

muatState();
</script>
</body>
</html>)html";

int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(HALAMAN, "text/html");
    });

    svr.Get("/state", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(buatResponse(), "application/json");
    });

    svr.Post("/enqueue", [](const httplib::Request& req, httplib::Response& res) {
        Mahasiswa mhs;
        mhs.nama    = ambilNilai(req.body, "nama");
        mhs.nim     = ambilNilai(req.body, "nim");
        mhs.judulTA = ambilNilai(req.body, "judul");
        mhs.status  = "Menunggu";

        if (mhs.nama.empty() || mhs.nim.empty() || mhs.judulTA.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"data tidak lengkap\"}", "application/json");
            return;
        }

        antrean.enqueue(mhs);
        cout << "Pendaftar baru masuk: " << mhs.nama << endl;
        res.set_content(buatResponse(), "application/json");
    });

    svr.Post("/verify", [](const httplib::Request& req, httplib::Response& res) {
        if (antrean.isEmpty()) {
            res.status = 400;
            res.set_content("{\"error\":\"antrean kosong\"}", "application/json");
            return;
        }

        bool setuju = req.body.find("true") != string::npos;
        Node* node = antrean.dequeue();
        node->data.status = setuju ? "Terverifikasi" : "Ditolak";
        riwayat.push(node);

        cout << "Verifikasi: " << node->data.nama << " -> " << node->data.status << endl;
        res.set_content(buatResponse(), "application/json");
    });

    cout << "Server jalan di http://localhost:8080" << endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
