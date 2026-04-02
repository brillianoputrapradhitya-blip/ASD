#ifndef STACK_H
#define STACK_H

#include "Mahasiswa.h"

class Stack {
private:
    Node* top;
    int   size;

public:
    Stack();
    ~Stack();

    void push(Node* node);
    void peek()          const;
    bool isEmpty()       const;
    int  getSize()       const;
    Node* getTop()       const;
    void tampilRiwayat() const;
};

#endif
