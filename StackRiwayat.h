#ifndef STACK_RIWAYAT_H
#define STACK_RIWAYAT_H

#include <vector>
#include "Pendaftar.h"

class StackRiwayat {
private:
    struct Node {
        Pendaftar data;
        Node* next;

        explicit Node(const Pendaftar& p) : data(p), next(nullptr) {}
    };

    Node* topNode;
    int panjang;

public:
    StackRiwayat();
    ~StackRiwayat();

    void push(const Pendaftar& pendaftar);
    bool peek(Pendaftar& outPendaftar) const;
    bool isEmpty() const;
    int size() const;
    std::vector<Pendaftar> toVector() const;
};

#endif
