#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "SistemPendadaran.h"

#ifdef _WIN32
using socket_t = SOCKET;
#else
using socket_t = int;
#endif

namespace {

struct AppState {
    SistemPendadaran sistem;
    std::vector<std::string> logs;
};

void closeSocket(socket_t sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

std::string nowTimeStr() {
    std::time_t t = std::time(nullptr);
    std::tm tmStruct{};
#ifdef _WIN32
    localtime_s(&tmStruct, &t);
#else
    localtime_r(&t, &tmStruct);
#endif
    std::ostringstream out;
    out << std::setfill('0') << std::setw(2) << tmStruct.tm_hour << ':' << std::setw(2) << tmStruct.tm_min << ':' << std::setw(2)
        << tmStruct.tm_sec;
    return out.str();
}

void pushLog(AppState& state, const std::string& type, const std::string& msg) {
    state.logs.insert(state.logs.begin(), nowTimeStr() + " [" + type + "] " + msg);
    if (state.logs.size() > 30) {
        state.logs.resize(30);
    }
}

std::string decodeUrl(const std::string& input) {
    std::ostringstream out;
    for (std::size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '+') {
            out << ' ';
        } else if (input[i] == '%' && i + 2 < input.size()) {
            std::string hex = input.substr(i + 1, 2);
            char ch = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            out << ch;
            i += 2;
        } else {
            out << input[i];
        }
    }
    return out.str();
}

std::map<std::string, std::string> parseQuery(const std::string& query) {
    std::map<std::string, std::string> params;
    std::istringstream ss(query);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
        std::size_t pos = pair.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        params[decodeUrl(pair.substr(0, pos))] = decodeUrl(pair.substr(pos + 1));
    }
    return params;
}

std::string htmlEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '&': out += "&amp;"; break;
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '"': out += "&quot;"; break;
            default: out += c; break;
        }
    }
    return out;
}

std::string statusClass(const std::string& status) {
    if (status == "Terverifikasi") {
        return "status-verified";
    }
    if (status == "Ditolak") {
        return "status-ditolak";
    }
    return "status-menunggu";
}

std::string renderPage(const AppState& state, const std::string& message) {
    auto antrean = state.sistem.lihatAntrean();
    auto riwayat = state.sistem.lihatRiwayat();

    int verifiedCount = 0;
    int rejectedCount = 0;
    for (const auto& p : riwayat) {
        if (p.statusVerifikasi == "Terverifikasi") {
            ++verifiedCount;
        } else if (p.statusVerifikasi == "Ditolak") {
            ++rejectedCount;
        }
    }

    std::ostringstream qHtml;
    if (antrean.empty()) {
        qHtml << "<div class='empty-state'><div class='big'>—</div>Antrean kosong</div>";
    } else {
        for (std::size_t i = 0; i < antrean.size(); ++i) {
            const auto& m = antrean[i];
            qHtml << "<div class='queue-item'>"
                  << "<div class='queue-num " << (i == 0 ? "first" : "") << "'>" << (i + 1) << "</div>"
                  << "<div><div class='queue-name'>" << htmlEscape(m.nama) << "</div>"
                  << "<div class='queue-nim'>" << htmlEscape(m.nim) << "</div>"
                  << "<div class='queue-judul'>" << htmlEscape(m.judulTugasAkhir) << "</div></div>"
                  << "<span class='status-tag status-menunggu'>Menunggu</span></div>";
        }
    }

    std::ostringstream sHtml;
    if (riwayat.empty()) {
        sHtml << "<div class='empty-state'><div class='big'>—</div>Belum ada yang diverifikasi</div>";
    } else {
        for (std::size_t i = 0; i < riwayat.size(); ++i) {
            const auto& m = riwayat[i];
            sHtml << "<div class='queue-item'>"
                  << "<div class='queue-num " << (i == 0 ? "first" : "") << "'>" << (i + 1) << "</div>"
                  << "<div><div class='queue-name'>" << htmlEscape(m.nama) << "</div>"
                  << "<div class='queue-nim'>" << htmlEscape(m.nim) << "</div>"
                  << "<div class='queue-judul'>" << htmlEscape(m.judulTugasAkhir) << "</div></div>"
                  << "<span class='status-tag " << statusClass(m.statusVerifikasi) << "'>" << htmlEscape(m.statusVerifikasi)
                  << "</span></div>";
        }
    }

    std::ostringstream verifArea;
    if (antrean.empty()) {
        verifArea << "<div class='verif-card muted'>Tidak ada pendaftar</div>";
    } else {
        const auto& next = antrean.front();
        verifArea << "<div class='verif-card highlight'><div class='verif-name'>" << htmlEscape(next.nama)
                  << "</div><div class='verif-nim'>" << htmlEscape(next.nim) << "</div><div class='verif-judul'>"
                  << htmlEscape(next.judulTugasAkhir) << "</div><div class='verif-actions'>"
                  << "<form method='GET' action='/verify'><input type='hidden' name='status' value='Terverifikasi'/><button class='btn-verify btn-approve' type='submit'>✓ Terverifikasi</button></form>"
                  << "<form method='GET' action='/verify'><input type='hidden' name='status' value='Ditolak'/><button class='btn-verify btn-reject' type='submit'>✗ Ditolak</button></form>"
                  << "</div></div>";
    }

    std::ostringstream peekArea;
    if (riwayat.empty()) {
        peekArea << "<div class='peek-card muted'>Stack kosong</div>";
    } else {
        const auto& top = riwayat.front();
        peekArea << "<div class='peek-card'><div class='peek-label'>top of stack</div><div class='queue-name'>" << htmlEscape(top.nama)
                 << "</div><div class='queue-nim'>" << htmlEscape(top.nim) << "</div><div class='queue-judul'>"
                 << htmlEscape(top.judulTugasAkhir) << "</div><span class='status-tag " << statusClass(top.statusVerifikasi) << "'>"
                 << htmlEscape(top.statusVerifikasi) << "</span></div>";
    }

    std::ostringstream logs;
    if (state.logs.empty()) {
        logs << "<div class='log-line'><span>Belum ada aktivitas</span></div>";
    } else {
        for (const auto& item : state.logs) {
            logs << "<div class='log-line'>" << htmlEscape(item) << "</div>";
        }
    }

    std::ostringstream out;
    out << "<!DOCTYPE html><html lang='id'><head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width, initial-scale=1.0'/>"
        << "<title>Sistem Antrean Sidang TA</title>"
        << "<style>*{box-sizing:border-box;margin:0;padding:0}:root{--bg:#0f0f0f;--bg2:#161616;--bg3:#1e1e1e;--border:#2a2a2a;--text:#e8e6e0;--muted:#777;--accent:#c8b560;--green:#4caf78;--red:#e05a5a;--blue:#5a9be0;--r:6px}"
        << "body{background:var(--bg);color:var(--text);font-family:Arial,sans-serif;font-size:14px;min-height:100vh;line-height:1.6}.shell{display:grid;grid-template-columns:260px 1fr 300px;grid-template-rows:56px 1fr;min-height:100vh}"
        << "header{grid-column:1/-1;background:var(--bg2);border-bottom:1px solid var(--border);display:flex;align-items:center;padding:0 24px;gap:16px}.logo-badge{font-size:11px;background:var(--accent);color:#0f0e08;padding:3px 8px;border-radius:3px;font-weight:700}.logo-title{font-size:17px;font-style:italic}.header-stats{margin-left:auto;display:flex;gap:24px;font-size:12px;color:var(--muted)}.header-stats b{color:var(--text)}"
        << ".sidebar-left,.sidebar-right{background:var(--bg2);padding:20px 16px;display:flex;flex-direction:column;gap:20px;overflow-y:auto}.sidebar-left{border-right:1px solid var(--border)}.sidebar-right{border-left:1px solid var(--border)}.section-label{font-size:10px;letter-spacing:.12em;text-transform:uppercase;color:var(--muted);margin-bottom:10px}"
        << "input,textarea{background:var(--bg3);border:1px solid var(--border);color:var(--text);padding:8px 10px;border-radius:var(--r);width:100%}textarea{height:65px}.btn{width:100%;padding:9px 14px;border-radius:var(--r);border:none;cursor:pointer;font-size:13px;font-weight:700}.btn-primary{background:var(--accent);color:#0f0e08}"
        << "main{padding:20px;overflow-y:auto;display:flex;flex-direction:column;gap:16px}.panel{background:var(--bg2);border:1px solid var(--border);border-radius:8px;overflow:hidden}.panel-header{padding:12px 16px;border-bottom:1px solid var(--border);display:flex;justify-content:space-between}.panel-title{font-size:11px;letter-spacing:.1em;text-transform:uppercase;color:var(--muted)}.count-badge{font-size:11px;color:var(--muted)}.panel-body{padding:12px 0}"
        << ".queue-item{display:grid;grid-template-columns:36px 1fr auto;gap:10px;padding:10px 16px;border-bottom:1px solid var(--border)}.queue-item:last-child{border-bottom:none}.queue-num{font-size:18px;color:#4a4a4a;text-align:center}.queue-num.first{color:var(--accent)}.queue-name{font-weight:700}.queue-nim{font-size:11px;color:var(--muted)}.queue-judul{font-size:12px;color:var(--muted)}"
        << ".status-tag{font-size:10px;padding:3px 8px;border-radius:3px;text-transform:uppercase}.status-menunggu{background:#1a1608;color:var(--accent)}.status-verified{background:#0d1f14;color:var(--green)}.status-ditolak{background:#1f0d0d;color:var(--red)}.empty-state{padding:32px 16px;text-align:center;color:#4a4a4a}.empty-state .big{font-size:28px}"
        << ".verif-card{background:var(--bg3);border:1px solid var(--border);border-radius:8px;padding:14px}.verif-card.highlight{border-color:var(--accent)}.verif-card.muted,.peek-card.muted{color:#4a4a4a;text-align:center;padding:20px 14px}.verif-name{font-size:15px;font-weight:700}.verif-nim{font-size:11px;color:var(--muted)}.verif-judul{font-size:12px;color:var(--muted);margin:8px 0 12px}.verif-actions{display:flex;gap:8px}.verif-actions form{flex:1}"
        << ".btn-verify{width:100%;padding:8px 10px;border-radius:var(--r);cursor:pointer;border:none;font-size:11px;font-weight:700}.btn-approve{background:#0d1f14;color:var(--green)}.btn-reject{background:#1f0d0d;color:var(--red)}.peek-card{background:var(--bg3);border:1px solid #383838;border-radius:8px;padding:14px}.peek-label{font-size:10px;letter-spacing:.1em;text-transform:uppercase;color:var(--muted);margin-bottom:10px}"
        << ".log-line{font-size:11px;color:var(--muted);padding:4px 0;border-bottom:1px solid var(--border)}.msg{background:#1e1e1e;border:1px solid var(--border);margin:12px 20px 0;padding:10px 12px;border-radius:6px;color:var(--blue)}@media(max-width:900px){.shell{grid-template-columns:1fr;grid-template-rows:auto}.sidebar-left,.sidebar-right{border:none;border-bottom:1px solid var(--border)}}"
        << "</style></head><body><div class='shell'><header><span class='logo-badge'>UGM</span><span class='logo-title'>Sistem Antrean Sidang Tugas Akhir</span><div class='header-stats'><span>Antrean: <b>"
        << antrean.size() << "</b></span><span>Terverifikasi: <b>" << verifiedCount << "</b></span><span>Ditolak: <b>" << rejectedCount
        << "</b></span></div></header>";

    out << "<aside class='sidebar-left'><div><div class='section-label'>Tambah Pendaftar</div><form method='GET' action='/add' style='display:flex;flex-direction:column;gap:10px'><input name='nama' type='text' placeholder='cth. Andi Saputra' required/><input name='nim' type='text' placeholder='cth. 22/123456/TK/12345' required/><textarea name='judul' placeholder='cth. Implementasi CNN untuk...' required></textarea><button class='btn btn-primary' type='submit'>+ Daftarkan ke Antrean</button></form></div><div style='flex:1'><div class='section-label'>Log Aktivitas</div><div>" << logs.str() << "</div></div></aside>";

    out << "<main>";
    if (!message.empty()) {
        out << "<div class='msg'>" << htmlEscape(message) << "</div>";
    }
    out << "<div class='panel'><div class='panel-header'><span class='panel-title'>Antrean Pendaftaran (Queue — FIFO)</span><span class='count-badge'>"
        << antrean.size() << " pendaftar</span></div><div class='panel-body'>" << qHtml.str() << "</div></div>";
    out << "<div class='panel'><div class='panel-header'><span class='panel-title'>Riwayat Verifikasi (Stack — LIFO)</span><span class='count-badge'>"
        << riwayat.size() << " data</span></div><div class='panel-body'>" << sHtml.str() << "</div></div></main>";

    out << "<aside class='sidebar-right'><div><div class='section-label'>Proses Verifikasi Berikutnya</div>" << verifArea.str()
        << "</div><div><div class='section-label'>Pendaftar Terakhir Diproses (Peek)</div>" << peekArea.str() << "</div></aside></div></body></html>";

    return out.str();
}

void sendResponse(socket_t clientSocket, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: text/html; charset=UTF-8\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "Connection: close\r\n\r\n"
             << body;

    std::string raw = response.str();
    send(clientSocket, raw.c_str(), static_cast<int>(raw.size()), 0);
}

}  // namespace

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup gagal.\n";
        return 1;
    }
#endif

    AppState state;

    socket_t serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == static_cast<socket_t>(-1)
#ifdef _WIN32
        || serverSocket == INVALID_SOCKET
#endif
    ) {
        std::cerr << "Gagal membuat socket.\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Bind gagal. Port 8080 mungkin sedang dipakai.\n";
        closeSocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Listen gagal.\n";
        closeSocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::cout << "Server berjalan di http://127.0.0.1:8080 (tanpa JavaScript)\n";

    while (true) {
        sockaddr_in clientAddr{};
#ifdef _WIN32
        int clientLen = sizeof(clientAddr);
#else
        socklen_t clientLen = sizeof(clientAddr);
#endif

        socket_t clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientSocket == static_cast<socket_t>(-1)
#ifdef _WIN32
            || clientSocket == INVALID_SOCKET
#endif
        ) {
            continue;
        }

        char buffer[8192];
        int received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) {
            closeSocket(clientSocket);
            continue;
        }
        buffer[received] = '\0';

        std::string request(buffer);
        std::istringstream reqStream{request};
        std::string method;
        std::string target;
        reqStream >> method >> target;

        std::string path = target;
        std::string query;
        std::size_t qpos = target.find('?');
        if (qpos != std::string::npos) {
            path = target.substr(0, qpos);
            query = target.substr(qpos + 1);
        }

        std::string message;
        auto params = parseQuery(query);

        if (path == "/add") {
            std::string nama = params["nama"];
            std::string nim = params["nim"];
            std::string judul = params["judul"];
            if (nama.empty() || nim.empty() || judul.empty()) {
                message = "Harap isi semua field.";
            } else {
                state.sistem.tambahPendaftar(nama, nim, judul);
                message = nama + " berhasil didaftarkan.";
                pushLog(state, "INF", "enqueue: " + nama);
            }
        } else if (path == "/verify") {
            Pendaftar out;
            std::string status = params["status"].empty() ? "Terverifikasi" : params["status"];
            if (state.sistem.verifikasiPendaftarBerikutnya(status, out)) {
                message = out.nama + " → " + status;
                pushLog(state, status == "Ditolak" ? "ERR" : "OK", "verified: " + out.nama + " -> " + status);
            } else {
                message = "Antrean kosong.";
            }
        }

        std::string body = renderPage(state, message);
        sendResponse(clientSocket, body);
        closeSocket(clientSocket);
    }

    closeSocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
