//
// Created by student on 14. 5. 2026..
//
#include "../h/scheduler.hpp"
#include "../h/ccb.hpp"

List<CCB> Scheduler::readyCoroutineQueue;

CCB *Scheduler::get() {
    return readyCoroutineQueue.removeFirst();
}

void Scheduler::put(CCB *ccb) {
    readyCoroutineQueue.addLast(ccb);
}
