#ifndef MAHASISWA_H
#define MAHASISWA_H

#include <string>
using namespace std;

struct Mahasiswa {
    string nama;
    string nim;
    string judulTA;
    string status;
};

struct Node {
    Mahasiswa data;
    Node* next;

    Node(Mahasiswa mhs) : data(mhs), next(nullptr) {}
};

#endif
