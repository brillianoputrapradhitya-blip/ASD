#ifndef PENDAFTAR_H
#define PENDAFTAR_H

#include <string>

struct Pendaftar {
    std::string nama;
    std::string nim;
    std::string judulTugasAkhir;
    std::string statusVerifikasi;

    Pendaftar() = default;

    Pendaftar(const std::string& nama,
              const std::string& nim,
              const std::string& judul,
              const std::string& status = "Belum Diverifikasi")
        : nama(nama), nim(nim), judulTugasAkhir(judul), statusVerifikasi(status) {}
};

#endif
