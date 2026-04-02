#include "SistemPendadaran.h"

void SistemPendadaran::tambahPendaftar(const std::string& nama,
                                       const std::string& nim,
                                       const std::string& judulTugasAkhir) {
    Pendaftar pendaftar(nama, nim, judulTugasAkhir, "Menunggu Verifikasi");
    antrean.enqueue(pendaftar);
}

bool SistemPendadaran::verifikasiPendaftarBerikutnya(const std::string& statusVerifikasi,
                                                     Pendaftar& outPendaftar) {
    if (!antrean.dequeue(outPendaftar)) {
        return false;
    }

    outPendaftar.statusVerifikasi = statusVerifikasi;
    riwayatVerifikasi.push(outPendaftar);
    return true;
}

bool SistemPendadaran::ambilPendaftarTerakhir(Pendaftar& outPendaftar) const {
    return riwayatVerifikasi.peek(outPendaftar);
}

std::vector<Pendaftar> SistemPendadaran::lihatAntrean() const {
    return antrean.toVector();
}

std::vector<Pendaftar> SistemPendadaran::lihatRiwayat() const {
    return riwayatVerifikasi.toVector();
}
