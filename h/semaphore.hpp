//
// Created by student on 10. 6. 2026..
//

#ifndef PROJECT_BASE_V1_1_SEMAPHORE_HPP
#define PROJECT_BASE_V1_1_SEMAPHORE_HPP
#include "../h/tcb.hpp"

class Semaphore
{
public:
    Semaphore(unsigned int init = 1):value((int)init),head(nullptr),tail(nullptr){}

    ~Semaphore();

    int wait();
    int signal();

    int wait_n(unsigned n);
    int signal_n(unsigned n );

private:
    unsigned int value; // SADA JE UNSIGNED (Nema više minusa)

    //Mini FIFO red blokiranih niti ugradjen direktno u semafor
    struct Node
    {
        TCB* thread;
        unsigned requested; // <--- NOVO: Pamtimo koliko resursa ova nit traži
        Node* next;
    };
    Node* head;
    Node* tail;

    void block(unsigned n);
    void deblock();
};

#endif //PROJECT_BASE_V1_1_SEMAPHORE_HPP
