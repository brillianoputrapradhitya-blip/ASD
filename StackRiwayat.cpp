#include "StackRiwayat.h"

StackRiwayat::StackRiwayat() : topNode(nullptr), panjang(0) {}

StackRiwayat::~StackRiwayat() {
    while (topNode != nullptr) {
        Node* nodeHapus = topNode;
        topNode = topNode->next;
        delete nodeHapus;
    }
}

void StackRiwayat::push(const Pendaftar& pendaftar) {
    Node* nodeBaru = new Node(pendaftar);
    nodeBaru->next = topNode;
    topNode = nodeBaru;
    ++panjang;
}

bool StackRiwayat::peek(Pendaftar& outPendaftar) const {
    if (isEmpty()) {
        return false;
    }

    outPendaftar = topNode->data;
    return true;
}

bool StackRiwayat::isEmpty() const {
    return topNode == nullptr;
}

int StackRiwayat::size() const {
    return panjang;
}

std::vector<Pendaftar> StackRiwayat::toVector() const {
    std::vector<Pendaftar> hasil;
    Node* current = topNode;

    while (current != nullptr) {
        hasil.push_back(current->data);
        current = current->next;
    }

    return hasil;
}
