// Server.cpp
// Compile (Windows MSYS2/MinGW):
//   g++ -std=c++11 -pthread Server.cpp -o server -lws2_32 -lwsock32
// Compile (Linux/Mac):
//   g++ -std=c++11 -pthread Server.cpp -o server
// Run and open: http://localhost:8080

#include "httplib.h"
#include <deque>
#include <iostream>
#include <string>
#include <vector>

struct Mahasiswa {
    std::string nama;
    std::string nim;
    std::string judul;
    std::string status;
};

std::deque<Mahasiswa> antrean;   // FIFO
std::vector<Mahasiswa> riwayat;  // LIFO (back = top)

// Very simple JSON value parser for payloads like {"nama":"..."}
std::string ambilNilai(const std::string& json, const std::string& key) {
    std::string cari = "\"" + key + "\":\"";
    size_t pos = json.find(cari);
    if (pos == std::string::npos) return "";
    pos += cari.size();
    size_t akhir = json.find("\"", pos);
    if (akhir == std::string::npos) return "";
    return json.substr(pos, akhir - pos);
}

std::string escapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else out += c;
    }
    return out;
}

std::string mhsKeJson(const Mahasiswa& m) {
    return "{\"nama\":\"" + escapeJson(m.nama) +
           "\",\"nim\":\"" + escapeJson(m.nim) +
           "\",\"judul\":\"" + escapeJson(m.judul) +
           "\",\"status\":\"" + escapeJson(m.status) + "\"}";
}

std::string buatStateJson() {
    std::string q = "[";
    for (size_t i = 0; i < antrean.size(); ++i) {
        if (i) q += ",";
        q += mhsKeJson(antrean[i]);
    }
    q += "]";

    std::string s = "[";
    for (size_t i = 0; i < riwayat.size(); ++i) {
        if (i) s += ",";
        // send top first to make UI easy
        const Mahasiswa& m = riwayat[riwayat.size() - 1 - i];
        s += mhsKeJson(m);
    }
    s += "]";

    return "{\"queue\":" + q +
           ",\"stack\":" + s +
           ",\"queueSize\":" + std::to_string(antrean.size()) +
           ",\"stackSize\":" + std::to_string(riwayat.size()) + "}";
}

const std::string HALAMAN = R"html(
<!doctype html>
<html lang="id">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Antrean Sidang TA - Simple</title>
  <style>
    body { font-family: Arial, sans-serif; max-width: 900px; margin: 20px auto; padding: 0 12px; }
    h1 { margin-bottom: 8px; }
    .row { display: flex; gap: 8px; flex-wrap: wrap; margin-bottom: 12px; }
    input { padding: 8px; width: 220px; }
    button { padding: 8px 12px; cursor: pointer; }
    .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 16px; }
    .box { border: 1px solid #ddd; border-radius: 6px; padding: 10px; }
    .item { border-bottom: 1px solid #eee; padding: 6px 0; }
    .item:last-child { border-bottom: none; }
    .muted { color: #666; font-size: 13px; }
  </style>
</head>
<body>
  <h1>Sistem Antrean Sidang TA</h1>
  <p class="muted">Versi sederhana (tanpa UI fancy).</p>

  <div class="row">
    <input id="nama" placeholder="Nama" />
    <input id="nim" placeholder="NIM" />
    <input id="judul" placeholder="Judul TA" />
    <button onclick="tambah()">Tambah</button>
  </div>

  <div class="row">
    <button onclick="verifikasi(true)">Verifikasi (Setuju)</button>
    <button onclick="verifikasi(false)">Verifikasi (Tolak)</button>
    <span id="stats" class="muted"></span>
  </div>

  <div class="grid">
    <div class="box">
      <h3>Queue (FIFO)</h3>
      <div id="queue"></div>
    </div>
    <div class="box">
      <h3>Riwayat (Stack / terbaru di atas)</h3>
      <div id="stack"></div>
    </div>
  </div>

<script>
async function req(url, data) {
  const opts = data
    ? { method: 'POST', headers: { 'Content-Type':'application/json' }, body: JSON.stringify(data) }
    : { method: 'GET' };
  const r = await fetch(url, opts);
  if (!r.ok) throw new Error(await r.text());
  return r.json();
}

function tampilList(id, arr, kosong) {
  const el = document.getElementById(id);
  if (!arr.length) { el.innerHTML = '<div class="muted">' + kosong + '</div>'; return; }
  el.innerHTML = arr.map((m, i) =>
    '<div class="item"><b>' + (i + 1) + '. ' + m.nama + '</b><br>' +
    '<span class="muted">' + m.nim + ' | ' + m.status + '</span><br>' +
    '<span>' + m.judul + '</span></div>'
  ).join('');
}

function render(s) {
  tampilList('queue', s.queue, 'Queue kosong');
  tampilList('stack', s.stack, 'Belum ada riwayat');
  document.getElementById('stats').textContent =
    'Queue: ' + s.queueSize + ' | Riwayat: ' + s.stackSize;
}

async function muat() {
  try { render(await req('/state')); }
  catch (e) { alert('Gagal memuat state: ' + e.message); }
}

async function tambah() {
  const nama = document.getElementById('nama').value.trim();
  const nim = document.getElementById('nim').value.trim();
  const judul = document.getElementById('judul').value.trim();
  if (!nama || !nim || !judul) return alert('Isi semua field dulu.');
  try {
    const s = await req('/enqueue', { nama, nim, judul });
    document.getElementById('nama').value = '';
    document.getElementById('nim').value = '';
    document.getElementById('judul').value = '';
    render(s);
  } catch (e) { alert('Gagal tambah: ' + e.message); }
}

async function verifikasi(approve) {
  try { render(await req('/verify', { approve })); }
  catch (e) { alert('Gagal verifikasi: ' + e.message); }
}

muat();
</script>
</body>
</html>
)html";

int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(HALAMAN, "text/html");
    });

    svr.Get("/state", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(buatStateJson(), "application/json");
    });

    svr.Post("/enqueue", [](const httplib::Request& req, httplib::Response& res) {
        Mahasiswa m;
        m.nama = ambilNilai(req.body, "nama");
        m.nim = ambilNilai(req.body, "nim");
        m.judul = ambilNilai(req.body, "judul");
        m.status = "Menunggu";

        if (m.nama.empty() || m.nim.empty() || m.judul.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"data tidak lengkap\"}", "application/json");
            return;
        }

        antrean.push_back(m);
        std::cout << "Enqueue: " << m.nama << std::endl;
        res.set_content(buatStateJson(), "application/json");
    });

    svr.Post("/verify", [](const httplib::Request& req, httplib::Response& res) {
        if (antrean.empty()) {
            res.status = 400;
            res.set_content("{\"error\":\"antrean kosong\"}", "application/json");
            return;
        }

        bool approve = req.body.find("true") != std::string::npos;

        Mahasiswa m = antrean.front();
        antrean.pop_front();
        m.status = approve ? "Terverifikasi" : "Ditolak";
        riwayat.push_back(m);

        std::cout << "Verify: " << m.nama << " -> " << m.status << std::endl;
        res.set_content(buatStateJson(), "application/json");
    });

    std::cout << "Server jalan di http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}
