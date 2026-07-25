//
// Created by student on 10. 6. 2026..
//

#ifndef PROJECT_BASE_V1_1_SEMAPHORE_HPP
#define PROJECT_BASE_V1_1_SEMAPHORE_HPP
#include "../h/tcb.hpp"

class _sem
{
public:
    _sem(unsigned int init = 1) : value(init), head(nullptr), tail(nullptr) {}

    ~_sem();

    int wait();
    int signal();

    int wait_n(unsigned n);
    int signal_n(unsigned n );

private:
    unsigned int value;

    //Mini FIFO red blokiranih niti ugradjen direktno u semafor
    struct Node
    {
        TCB* thread;
        unsigned requested;
        Node* next;
    };
    Node* head;
    Node* tail;

    int block(unsigned n);
    void deblock(int status = 0);
};

#endif //PROJECT_BASE_V1_1_SEMAPHORE_HPP
