//
// Created by student on 10. 6. 2026..
//
#include "../h/_sem.hpp"
#include "../h/scheduler.hpp"

_sem::~_sem() {
    // Ako se semafor gasi, moramo odblokirati sve niti iz reda da ne ostanu zauvek zaglavljene
    while (head) {
        deblock(-1);
    }
}


// Običan wait i signal su sada samo specijalni slučajevi za n=1 !
int _sem::wait() { return wait_n(1); }
int _sem::signal() { return signal_n(1); }


int _sem::wait_n(unsigned int n)
{
    if (n==0) return 0;// Ako traži 0 resursa, odmah vraća uspeh

    if (value>=n)
    {
        value-=(int)n;
        return 0;// Ima dovoljno resursa, proleti dalje
    }else
    {
        return block(n);// Nemamo dovoljno, nit se blokira i pamti "n"
    }
}

int _sem::signal_n(unsigned int n)
{
    if (n==0) return 0;
    value += n; // Dodajemo nove resurse

    // Budi niti iz reda SVE DOK imamo dovoljno resursa da ispunimo zahtev prve niti!
    while (head != nullptr && value>=head->requested)
    {
        value-=head->requested;// Unapred joj oduzimamo resurse
        deblock(0);// Budimo je uspesno
    }
    return 0;
}

int _sem::block(unsigned n)
{
    TCB* old = TCB::running;

    if (old == nullptr)
    {
        return -1;
    }

    // 1. Trenutnu nit stavljamo u red ovog semafora
    Node* newNode = new Node{old, n,nullptr};
    if (newNode == nullptr)
    {
        return -1;
    }
    TCB* next=Scheduler::get();
    if (next == nullptr)
    {
        delete newNode;
        return -1;
    }
    if (!head) {
        head = newNode;
    } else {
        tail->next = newNode;
    }

    tail = newNode;

    old->setSemaphoreStatus(0);
    old->setState(TCB::BLOCKED);

    TCB::running=next;
    TCB::running->setState(TCB::RUNNING);

    TCB::contextSwitch(&old->context, &TCB::running->context);

    return old->getSemaphoreStatus();
}

void _sem::deblock(int status)
{
    if (!head) return;
    // 1. Skidamo prvu nit sa reda semafora
    Node*temp = head;
    head = head->next;
    if (!head) tail = nullptr;

    if (temp->thread != nullptr)
    {
        // 2. Vraćamo je nazad u Scheduler da nastavi sa radom
        temp->thread->setSemaphoreStatus(status);
        temp->thread->setState(TCB::READY);
        Scheduler::put(temp->thread);
    }

    delete temp;
}
