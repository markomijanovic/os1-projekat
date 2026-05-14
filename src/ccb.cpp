//
// Created by student on 14. 5. 2026..
//

#include "../h/ccb.hpp"

#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"

//extern "C" void pushRegisters();
CCB *CCB::running=nullptr;

CCB* CCB::createCoroutine(Body body) {
    return new CCB(body);
}

void CCB::yield() {
    Riscv::pushRegisters();
    CCB::dispatch();
    Riscv::popRegisters();
}

void CCB::dispatch() {
    CCB* old = running;
    if (!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    CCB::contextSwitch(&old->context, &running->context);

}