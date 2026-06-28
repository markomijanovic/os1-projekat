//
// Created by student on 14. 5. 2026..
//

#include "../h/tcb.hpp"

#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"
#include "../h/syscall_c.hpp"

//extern "C" void pushRegisters();
TCB *TCB::running = nullptr;
TCB *TCB::zombie = nullptr;

// Inicijalizacija niza i brojaca
TCB* TCB::sleepQueue[1000];
int TCB::sleepCount = 0;
uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body,void * arg, bool kernelThread)
{
    return new TCB(body,arg, TIME_SLICE, kernelThread);
}

void TCB::yield()
{
    // Koristimo sirovi asembler da nasilno upišemo 0x13 u a0.
    // Na ovaj način sprečavamo GCC prevodilac da optimizuje i obriše vrednost!
    __asm__ volatile ("li a0, 0x13");
    __asm__ volatile ("ecall");
}

void TCB::dispatch()
{
    if (zombie != nullptr)
    {
        delete zombie;
        zombie = nullptr;
    }

    TCB *old = running;

    if (old !=nullptr && old->getState()== RUNNING)
    {
        old->setState(READY);
        Scheduler::put(old);
    }
    else if ( old != nullptr && old->getState()== FINISHED)
    {
        zombie = old;
    }

    running = Scheduler::get();

    // DODATA ZAŠTITA: Ako je čekaonica prazna, ne radi ništa!
    if (running == nullptr) return;

    running->setState(RUNNING);

    if (old != nullptr)
    {
        TCB::contextSwitch(&old->context, &running->context);
    }
}

void TCB::threadWrapper()
{
    if (running->isKernelThread())
    {
        Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    }
    else
    {
        Riscv::popSppSpie();
    }

    if (running->body)
    {
        // Ažurirano: Sada prosleđujemo argument funkciji!
        running->body(running->arg);
    }
    // Ažurirano: Umesto ručnog gašenja, sada zovemo naš novi sistemski poziv!
    thread_exit();
}

int TCB::sleep(time_t ticks)
{
    if (ticks == 0) return 0;

    if (TCB::running == nullptr)
    {
        return -1;
    }

    TCB::running->sleepTime = ticks;
    TCB::running->setState(SLEEPING);

    // Ubacujemo nit u niz uspavanih
    sleepQueue[sleepCount++] = TCB::running;

    // Menjamo kontekst
    TCB* old = TCB::running;
    TCB::running = Scheduler::get();
    if (TCB::running != nullptr) {
        TCB::running->setState(RUNNING);
        TCB::contextSwitch(&old->context, &TCB::running->context);
    }
    return 0;
}

void TCB::updateSleepingThreads()
{
    for (int i = 0; i < sleepCount; ) {
        sleepQueue[i]->sleepTime--;

        if (sleepQueue[i]->sleepTime == 0) {
            // Vreme je isteklo! Vracamo nit u Scheduler
            sleepQueue[i]->setState(READY);
            Scheduler::put(sleepQueue[i]);

            // Brzo brisanje iz niza: prevucemo poslednji element na ovo mesto
            sleepQueue[i] = sleepQueue[--sleepCount];
        } else {
            // Ako i dalje spava, samo idemo na sledecu nit
            i++;
        }
    }
}
