//
// Created by student on 14. 5. 2026..
//

#include "../h/tcb.hpp"

#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"
#include "../h/syscall_c.hpp"

//extern "C" void pushRegisters();
TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body,void * arg)
{
    return new TCB(body,arg, TIME_SLICE);
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
    TCB *old = running;
    if (!old->isFinished())
    {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    // DODATA ZAŠTITA: Ako je čekaonica prazna, ne radi ništa!
    if (running == nullptr) return;

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    // OBRISANO: Riscv::popSppSpie(); jer 'sret' pravi haos sa privilegijama!

    // Umesto toga, samo ručno palimo prekide za novu nit:
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    if (running->body)
    {
        // Ažurirano: Sada prosleđujemo argument funkciji!
        running->body(running->arg);
    }
    // Ažurirano: Umesto ručnog gašenja, sada zovemo naš novi sistemski poziv!
    thread_exit();
}