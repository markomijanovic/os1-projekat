//
// Created by student on 14. 5. 2026..
//
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/syscall_c.hpp" // KLJUČNA IZMENA: Uključujemo sistemske pozive za niti

void workerBodyA(void* arg) // KLJUČNA IZMENA: Dodat void* arg
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
            // thread_dispatch(); // Ako želiš da otkomentarišeš, sada ide ovako
        }
    }
}

void workerBodyB(void* arg) // KLJUČNA IZMENA: Dodat void* arg
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
            // thread_dispatch();
        }
    }
}

static uint64 fibbonacci(uint64 n)
{
    if (n == 0 || n == 1) return n;

    if (n % 4 == 0) thread_dispatch(); // KLJUČNA IZMENA: Zamenjen TCB::yield()

    return fibbonacci(n - 1) + fibbonacci(n - 2);
}

void workerBodyC(void* arg) // KLJUČNA IZMENA: Dodat void* arg
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

    thread_dispatch(); // KLJUČNA IZMENA: Zamenjen TCB::yield()

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
}

void workerBodyD(void* arg) // KLJUČNA IZMENA: Dodat void* arg
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

    thread_dispatch(); // KLJUČNA IZMENA: Zamenjen TCB::yield()

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
}