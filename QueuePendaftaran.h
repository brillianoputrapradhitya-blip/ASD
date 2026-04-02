#ifndef QUEUE_PENDAFTARAN_H
#define QUEUE_PENDAFTARAN_H

#include <vector>
#include "Pendaftar.h"

class QueuePendaftaran {
private:
    struct Node {
        Pendaftar data;
        Node* next;

        explicit Node(const Pendaftar& p) : data(p), next(nullptr) {}
    };

    Node* frontNode;
    Node* rearNode;
    int panjang;

public:
    QueuePendaftaran();
    ~QueuePendaftaran();

    void enqueue(const Pendaftar& pendaftar);
    bool dequeue(Pendaftar& outPendaftar);
    bool isEmpty() const;
    int size() const;
    std::vector<Pendaftar> toVector() const;
};

#endif
