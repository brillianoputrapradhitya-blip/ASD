#include "QueuePendaftaran.h"

QueuePendaftaran::QueuePendaftaran() : frontNode(nullptr), rearNode(nullptr), panjang(0) {}

QueuePendaftaran::~QueuePendaftaran() {
    Pendaftar temp;
    while (dequeue(temp)) {
    }
}

void QueuePendaftaran::enqueue(const Pendaftar& pendaftar) {
    Node* nodeBaru = new Node(pendaftar);

    if (rearNode == nullptr) {
        frontNode = rearNode = nodeBaru;
    } else {
        rearNode->next = nodeBaru;
        rearNode = nodeBaru;
    }

    ++panjang;
}

bool QueuePendaftaran::dequeue(Pendaftar& outPendaftar) {
    if (isEmpty()) {
        return false;
    }

    Node* nodeHapus = frontNode;
    outPendaftar = nodeHapus->data;
    frontNode = frontNode->next;

    if (frontNode == nullptr) {
        rearNode = nullptr;
    }

    delete nodeHapus;
    --panjang;
    return true;
}

bool QueuePendaftaran::isEmpty() const {
    return frontNode == nullptr;
}

int QueuePendaftaran::size() const {
    return panjang;
}

std::vector<Pendaftar> QueuePendaftaran::toVector() const {
    std::vector<Pendaftar> hasil;
    Node* current = frontNode;

    while (current != nullptr) {
        hasil.push_back(current->data);
        current = current->next;
    }

    return hasil;
}
