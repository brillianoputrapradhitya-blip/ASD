#ifndef QUEUE_H
#define QUEUE_H

#include "Mahasiswa.h"

class Queue {
private:
    Node* head;
    Node* tail;
    int   size;

public:
    Queue();
    ~Queue();

    void  enqueue(Mahasiswa mhs);
    Node* dequeue();
    bool  isEmpty()  const;
    int   getSize()  const;
    Node* getHead()  const;
    void  tampilAntrean() const;
};

#endif
