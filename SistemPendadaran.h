#ifndef SISTEM_PENDADARAN_H
#define SISTEM_PENDADARAN_H

#include <string>
#include "QueuePendaftaran.h"
#include "StackRiwayat.h"

class SistemPendadaran {
private:
    QueuePendaftaran antrean;
    StackRiwayat riwayatVerifikasi;

public:
    void tambahPendaftar(const std::string& nama,
                         const std::string& nim,
                         const std::string& judulTugasAkhir);

    bool verifikasiPendaftarBerikutnya(const std::string& statusVerifikasi, Pendaftar& outPendaftar);
    bool ambilPendaftarTerakhir(Pendaftar& outPendaftar) const;

    std::vector<Pendaftar> lihatAntrean() const;
    std::vector<Pendaftar> lihatRiwayat() const;
};

#endif
