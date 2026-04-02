#include "Queue.h"
#include <iostream>
using namespace std;

Queue::Queue() : head(nullptr), tail(nullptr), size(0) {}

Queue::~Queue() {
    while (!isEmpty()) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

void Queue::enqueue(Mahasiswa mhs) {
    Node* nodeBaru = new Node(mhs);
    if (tail == nullptr) {
        head = tail = nodeBaru;
    } else {
        tail->next = nodeBaru;
        tail       = nodeBaru;
    }
    size++;
    cout << "[Queue] " << mhs.nama << " (" << mhs.nim << ") berhasil didaftarkan ke antrean.\n";
}

Node* Queue::dequeue() {
    if (isEmpty()) {
        cout << "[Queue] Antrean kosong. Tidak ada pendaftar yang dapat diproses.\n";
        return nullptr;
    }
    Node* temp = head;
    head = head->next;
    if (head == nullptr) tail = nullptr;
    temp->next = nullptr;
    size--;
    return temp;
}

bool Queue::isEmpty() const {
    return head == nullptr;
}

int Queue::getSize() const {
    return size;
}

Node* Queue::getHead() const {
    return head;
}

void Queue::tampilAntrean() const {
    if (isEmpty()) {
        cout << "  (Antrean kosong)\n";
        return;
    }
    Node* curr  = head;
    int urutan  = 1;
    while (curr != nullptr) {
        cout << "  " << urutan++ << ". "
             << curr->data.nama  << "\n"
             << "     NIM   : " << curr->data.nim << "\n"
             << "     Judul : " << curr->data.judulTA << "\n"
             << "     Status: " << curr->data.status << "\n\n";
        curr = curr->next;
    }
}
