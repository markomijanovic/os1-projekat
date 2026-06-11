//
// Created by student on 10. 6. 2026..
//
#include "../h/_sem.hpp"
#include "../h/scheduler.hpp"

_sem::~_sem() {
    // Ako se semafor gasi, moramo odblokirati sve niti iz reda da ne ostanu zauvek zaglavljene
    while (head) {
        deblock();
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
        block(n);// Nemamo dovoljno, nit se blokira i pamti "n"
        return 0;
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
        deblock();// Budimo je!
    }
    return 0;
}

void _sem::block(unsigned n) {
    // 1. Trenutnu nit stavljamo u red ovog semafora
    Node* newNode = new Node{TCB::running, n,nullptr};
    if (!head) {
        head = newNode;
    } else {
        tail->next = newNode;
    }
    tail = newNode;

    // 2. Ručno prebacujemo kontekst BEZ vracanja u Scheduler
    TCB* old = TCB::running;
    TCB::running = Scheduler::get();

    // Zastita od nule (ako nema vise niti za rad)
    if (TCB::running != nullptr) {
        TCB::contextSwitch(&old->context, &TCB::running->context);
    }
}

void _sem::deblock()
{
    if (!head) return;
    // 1. Skidamo prvu nit sa reda semafora
    Node*temp = head;
    head = head->next;
    if (!head) tail = nullptr;

    // 2. Vraćamo je nazad u Scheduler da nastavi sa radom
    Scheduler::put(temp->thread);
    delete temp;
}
