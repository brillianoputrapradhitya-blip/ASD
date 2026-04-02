#include "Stack.h"
#include <iostream>
using namespace std;

Stack::Stack() : top(nullptr), size(0) {}

Stack::~Stack() {
    while (top != nullptr) {
        Node* temp = top;
        top        = top->next;
        delete temp;
    }
}

void Stack::push(Node* node) {
    node->next = top;
    top        = node;
    size++;
}

void Stack::peek() const {
    if (isEmpty()) {
        cout << "  (Riwayat masih kosong)\n";
        return;
    }
    cout << "  Nama   : " << top->data.nama << "\n"
         << "  NIM    : " << top->data.nim << "\n"
         << "  Judul  : " << top->data.judulTA << "\n"
         << "  Status : " << top->data.status << "\n";
}

bool Stack::isEmpty() const {
    return top == nullptr;
}

int Stack::getSize() const {
    return size;
}

Node* Stack::getTop() const {
    return top;
}

void Stack::tampilRiwayat() const {
    if (isEmpty()) {
        cout << "  (Riwayat kosong)\n";
        return;
    }
    Node* curr  = top;
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
