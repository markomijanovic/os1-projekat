//
// Created by student on 14. 5. 2026..
//
#include "../h/ccb.hpp"
#include "../h/print.hpp"
#include "../h/workers.hpp"

int main() {

    CCB* coroutines[3];

    coroutines[0] = CCB::createCoroutine(nullptr);
    CCB::running=coroutines[0];
    coroutines[1] = CCB::createCoroutine(workerBodyA);
    printString("Coroutine A created\n");
    coroutines[2] = CCB::createCoroutine(workerBodyB);
    printString("Coroutine B created\n");
    while (!(coroutines[1]->isFinished() && coroutines[2]->isFinished())) {
        CCB::yield();
    }


    for (auto &coroutine : coroutines) {
        delete coroutine;
    }
    printString("Finished\n");
    return 0;

}