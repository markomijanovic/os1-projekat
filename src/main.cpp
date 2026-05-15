//
// Created by student on 14. 5. 2026..
//
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/workers.hpp"
#include "../h/riscv.hpp"

int main() {

    TCB* threads[5];

    threads[0] = TCB::createThread(nullptr);
    TCB::running=threads[0];
    threads[1] = TCB::createThread(workerBodyA);
    printString("Thread A created\n");
    threads[2] = TCB::createThread(workerBodyB);
    printString("Thread B created\n");
    threads[3] = TCB::createThread(workerBodyC);
    printString("Thread C created\n");
    threads[4] = TCB::createThread(workerBodyD);
    printString("Thread D created\n");

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while (!(threads[1]->isFinished() &&
            threads[2]->isFinished() &&
            threads[3]->isFinished() &&
            threads[4]->isFinished())) {
        TCB::yield();
    }


    for (auto &coroutine : threads) {
        delete coroutine;
    }
    printString("Finished\n");
    return 0;

}