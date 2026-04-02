#include <iostream>
#include <limits>
#include <vector>
#include "include/SistemPendadaran.h"

void tampilkanPendaftar(const Pendaftar& p, int nomor) {
    std::cout << nomor << ". Nama   : " << p.nama << '\n';
    std::cout << "   NIM    : " << p.nim << '\n';
    std::cout << "   Judul  : " << p.judulTugasAkhir << '\n';
    std::cout << "   Status : " << p.statusVerifikasi << "\n\n";
}

int bacaPilihan() {
    int pilihan;
    std::cout << "Pilih menu: ";
    std::cin >> pilihan;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return pilihan;
}

int main() {
    SistemPendadaran sistem;
    int pilihan;

    do {
        std::cout << "==== Sistem Antrean Pendadaran ====" << '\n';
        std::cout << "1. Tambah pendaftar" << '\n';
        std::cout << "2. Proses verifikasi pendaftar berikutnya" << '\n';
        std::cout << "3. Tampilkan pendaftar terakhir diproses" << '\n';
        std::cout << "4. Tampilkan antrean pendaftaran" << '\n';
        std::cout << "5. Tampilkan riwayat verifikasi" << '\n';
        std::cout << "0. Keluar" << '\n';

        pilihan = bacaPilihan();

        switch (pilihan) {
            case 1: {
                std::string nama;
                std::string nim;
                std::string judul;

                std::cout << "Nama mahasiswa: ";
                std::getline(std::cin, nama);
                std::cout << "NIM: ";
                std::getline(std::cin, nim);
                std::cout << "Judul tugas akhir: ";
                std::getline(std::cin, judul);

                sistem.tambahPendaftar(nama, nim, judul);
                std::cout << "Pendaftar berhasil ditambahkan ke antrean.\n\n";
                break;
            }

            case 2: {
                std::string status;
                Pendaftar terverifikasi;

                std::cout << "Masukkan status verifikasi (Diterima / Perlu Revisi / Ditolak): ";
                std::getline(std::cin, status);

                if (sistem.verifikasiPendaftarBerikutnya(status, terverifikasi)) {
                    std::cout << "Pendaftar berhasil diverifikasi:\n";
                    tampilkanPendaftar(terverifikasi, 1);
                } else {
                    std::cout << "Antrean kosong, tidak ada data untuk diverifikasi.\n\n";
                }
                break;
            }

            case 3: {
                Pendaftar terakhir;
                if (sistem.ambilPendaftarTerakhir(terakhir)) {
                    std::cout << "Pendaftar terakhir yang diproses:\n";
                    tampilkanPendaftar(terakhir, 1);
                } else {
                    std::cout << "Belum ada riwayat verifikasi.\n\n";
                }
                break;
            }

            case 4: {
                std::vector<Pendaftar> antrean = sistem.lihatAntrean();
                if (antrean.empty()) {
                    std::cout << "Antrean pendaftaran kosong.\n\n";
                } else {
                    std::cout << "Daftar antrean pendaftaran (FIFO):\n\n";
                    for (std::size_t i = 0; i < antrean.size(); ++i) {
                        tampilkanPendaftar(antrean[i], static_cast<int>(i + 1));
                    }
                }
                break;
            }

            case 5: {
                std::vector<Pendaftar> riwayat = sistem.lihatRiwayat();
                if (riwayat.empty()) {
                    std::cout << "Riwayat verifikasi kosong.\n\n";
                } else {
                    std::cout << "Riwayat verifikasi (LIFO - terbaru di atas):\n\n";
                    for (std::size_t i = 0; i < riwayat.size(); ++i) {
                        tampilkanPendaftar(riwayat[i], static_cast<int>(i + 1));
                    }
                }
                break;
            }

            case 0:
                std::cout << "Program selesai.\n";
                break;

            default:
                std::cout << "Pilihan tidak valid.\n\n";
                break;
        }

    } while (pilihan != 0);

    return 0;
}
