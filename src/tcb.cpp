//
// Created by student on 14. 5. 2026..
//

#include "../h/tcb.hpp"

#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"

//extern "C" void pushRegisters();
TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body)
{
    return new TCB(body, TIME_SLICE);
}

void TCB::yield()
{
    __asm__ volatile ("ecall"); // ovdje skacemo u prekidnu rutinu gdje radimo promenu konteksta
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished())
    {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body();
    running->setFinished(true);
    TCB::yield();
}