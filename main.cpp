#include <iostream>
#include <limits>
#include "Queue.h"
#include "Stack.h"
using namespace std;


void cetakGaris(char c = '-', int n = 54) {
    cout << string(n, c) << "\n";
}

void tampilMenu(const Queue& q, const Stack& s) {
    cetakGaris('=');
    cout << "  SISTEM ANTREAN SIDANG TUGAS AKHIR (PENDADARAN)\n";
    cetakGaris('=');
    cout << "  Pendaftar dalam antrean : " << q.getSize() << "\n";
    cout << "  Total sudah diverifikasi: " << s.getSize() << "\n";
    cetakGaris();
    cout << "  1. Tambah pendaftar ke antrean\n";
    cout << "  2. Proses verifikasi pendaftar berikutnya\n";
    cout << "  3. Tampilkan antrean pendaftaran\n";
    cout << "  4. Tampilkan riwayat verifikasi\n";
    cout << "  5. Tampilkan pendaftar terakhir yang diproses\n";
    cout << "  0. Keluar\n";
    cetakGaris();
    cout << "  Pilihan: ";
}

void tambahPendaftar(Queue& q) {
    Mahasiswa mhs;
    cout << "\n--- Tambah Pendaftar ---\n";
    cout << "Nama lengkap : "; getline(cin, mhs.nama);
    cout << "NIM          : "; getline(cin, mhs.nim);
    cout << "Judul TA     : "; getline(cin, mhs.judulTA);
    mhs.status = "Menunggu";
    q.enqueue(mhs);
}

void prosesVerifikasi(Queue& q, Stack& s) {
    cout << "\n--- Proses Verifikasi ---\n";

    Node* node = q.dequeue();
    if (node == nullptr) return;

    cout << "Memproses : " << node->data.nama << " (" << node->data.nim << ")\n";
    cout << "Judul TA  : " << node->data.judulTA << "\n\n";

    string pilStatus;
    cout << "Hasil verifikasi berkas:\n";
    cout << "  1. Terverifikasi\n";
    cout << "  2. Ditolak\n";
    cout << "Pilih (1/2): ";
    getline(cin, pilStatus);

    if (pilStatus == "1") {
        node->data.status = "Terverifikasi";
    } else if (pilStatus == "2") {
        node->data.status = "Ditolak";
    } else {
        node->data.status = "Tidak Diketahui";
        cout << "[!] Input tidak valid, status diset ke 'Tidak Diketahui'.\n";
    }

    s.push(node);
    cout << "[Stack] Data " << node->data.nama
         << " disimpan ke riwayat dengan status: "
         << node->data.status << "\n";
}


int main() {
    Queue antrean;
    Stack riwayat;
    int   pilihan;

    do {
        cout << "\n";
        tampilMenu(antrean, riwayat);

        cin >> pilihan;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "\n";

        switch (pilihan) {

            case 1:
                tambahPendaftar(antrean);
                break;

            case 2:
                prosesVerifikasi(antrean, riwayat);
                break;

            case 3:
                cout << "=== ANTREAN PENDAFTARAN ===\n";
                cout << "Jumlah dalam antrean: " << antrean.getSize() << "\n\n";
                antrean.tampilAntrean();
                break;

            case 4:
                cout << "=== RIWAYAT VERIFIKASI ===\n";
                cout << "Total diverifikasi: " << riwayat.getSize() << "\n\n";
                riwayat.tampilRiwayat();
                break;

            case 5:
                cout << "=== PENDAFTAR TERAKHIR YANG DIPROSES ===\n\n";
                riwayat.peek();
                break;

            case 0:
                cout << "Program selesai. Terima kasih.\n";
                break;

            default:
                cout << "[!] Pilihan tidak valid. Silakan pilih 0-5.\n";
        }

        if (pilihan != 0) {
            cout << "\nTekan Enter untuk kembali ke menu...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

    } while (pilihan != 0);

    return 0;
}
