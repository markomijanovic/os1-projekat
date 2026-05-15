//
// Created by student on 14. 5. 2026..
//
#include "../h/workers.hpp"
#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/scheduler.hpp"


void workerBodyA()
{
    for (uint64 i = 0; i < 10; i++)
    {
        printString("A: i=");
        printInteger(i);
        printString("\n");
        for (uint64 j = 0; j < 10000; j++)
        {
            for (uint64 k = 0; k < 10000; k++)
            {
                // bussy wait
            }
            // TCB::yield();
        }
    }
}

void workerBodyB()
{
     for (uint64 i = 0; i < 16; i++)
    {
        printString("B: i=");
        printInteger(i);
        printString("\n");
        for (uint64 j = 0; j < 10000; j++)
        {
            for (uint64 k = 0; k < 10000; k++)
            {
                // bussy wait
            }
            // TCB::yield();
        }
    }
}

static uint64 fibbonacci(uint64 n)
{
    if (n == 0 || n == 1) return n;
    if (n % 4 == 0) TCB::yield();

    return fibbonacci(n - 1) + fibbonacci(n - 2);
}

void workerBodyC()
{
    uint8 i = 0;
    for (; i < 3; i++)
    {
        printString("A: i=");
        printInteger(i);
        printString("\n");
    }

    printString("C: yield\n");
    __asm__ ("li t1,7");
    TCB::yield();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1":[t1]"=r"(t1));
    printString("A: t1=");
    printInteger(t1);
    printString("\n");

    uint64 result = fibbonacci(20);
    printString("A: fibbonacci=");
    printInteger(result);
    printString("\n");

    for (; i < 6; i++)
    {
        printString("A: i=");
        printInteger(i);
        printString("\n");
    }
    // CCB::running->setFinished(true);
    // TCB::yield();
}

void workerBodyD()
{
    uint8 i = 10;
    for (; i < 13; i++)
    {
        printString("D: i=");
        printInteger(i);
        printString("\n");
    }
    printString("B: yield\n");
    __asm__ ("li t1,5");
    TCB::yield();

    uint64 result = fibbonacci(23);
    printString("D: fibbonacci=");
    printInteger(result);
    printString("\n");

    for (; i < 16; i++)
    {
        printString("B: i=");


        printInteger(i);
        printString("\n");
    }
    // CCB::running->setFinished(true);
    // TCB::yield();
}
