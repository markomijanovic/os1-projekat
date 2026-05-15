//
// Created by student on 14. 5. 2026..
//
#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

List<TCB> Scheduler::readyCoroutineQueue;

TCB *Scheduler::get() {
    return readyCoroutineQueue.removeFirst();
}

void Scheduler::put(TCB *ccb) {
    readyCoroutineQueue.addLast(ccb);
}
