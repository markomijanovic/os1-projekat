//
// Created by student on 14. 5. 2026..
//
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/syscall_c.hpp"

extern sem_t semafor_zavrsetka;
void workerBodyA(void* arg) {
    for (uint64 i = 0; i < 10; i++) {
        printString("A: i=");
        printInteger(i);
        printString("\n");

        time_sleep(5);
    }
    sem_signal(semafor_zavrsetka);
    printString("Worker A uspesno poslao signal!\n");
}

void workerBodyB(void* arg) {
    for (uint64 i = 0; i < 10; i++) {
        printString("B: i=");
        printInteger(i);
        printString("\n");

        time_sleep(3);
    }
    sem_signal(semafor_zavrsetka);
    printString("Worker B uspesno poslao signal!\n");
}

static uint64 fibbonacci(uint64 n)
{
    if (n == 0 || n == 1) return n;

    if (n % 4 == 0) thread_dispatch();

    return fibbonacci(n - 1) + fibbonacci(n - 2);
}

void workerBodyC(void* arg)
{
    uint8 i = 0;
    for (; i < 3; i++)
    {
        printString("C: i=");
        printInteger(i);
        printString("\n");
    }

    printString("C: yield\n");
    __asm__ ("li t1,7");

    thread_dispatch();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1":[t1]"=r"(t1));
    printString("C: t1=");
    printInteger(t1);
    printString("\n");

    uint64 result = fibbonacci(20);
    printString("C: fibbonacci=");
    printInteger(result);
    printString("\n");

    for (; i < 6; i++)
    {
        printString("C: i=");
        printInteger(i);
        printString("\n");
    }
    sem_signal(semafor_zavrsetka);
    printString("Worker C uspesno poslao signal!\n");
}

void workerBodyD(void* arg)
{
    uint8 i = 10;
    for (; i < 13; i++)
    {
        printString("D: i=");
        printInteger(i);
        printString("\n");
    }
    printString("D: yield\n");
    __asm__ ("li t1,5");

    thread_dispatch();

    uint64 result = fibbonacci(23);
    printString("D: fibbonacci=");
    printInteger(result);
    printString("\n");

    for (; i < 16; i++)
    {
        printString("D: i=");
        printInteger(i);
        printString("\n");
    }
    sem_signal(semafor_zavrsetka);
    printString("Worker D uspesno poslao signal!\n");
}
