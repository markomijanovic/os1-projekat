
#include "../h/syscall_c.hpp"


void * mem_alloc(size_t size)
{
    // 1. Pretvaramo bajtove u blokove (zaokruživanje na gore)
    size_t blocks= size / MEM_BLOCK_SIZE;
    if (size % MEM_BLOCK_SIZE)
    {
        blocks++;
    }
    // 2. Pakujemo registre prema ABI specifikaciji
    // a0: Kod sistemskog poziva (0x01 za mem_alloc)
    // a1: Broj blokova
    register uint64 a0 asm ("a0")=0x01;
    register uint64 a1 asm ("a1")=blocks;

    // 3. Softverski prekid
    __asm__ volatile ("ecall" : "+r"(a0):"r"(a1):"memory");

    //Povratna vrednost iz kernela vracena kroz a0
    return (void*) a0;

}
int mem_free(void* p) {
    register uint64 a0 asm("a0") = 0x02;
    register uint64 a1 asm("a1") = (uint64)p;

    __asm__ volatile ("ecall" : "+r" (a0) : "r" (a1) : "memory");

    return (int)a0;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg)
{
    register uint64 a0 asm ("a0") = 0x11;
    register uint64 a1 asm ("a1") = (uint64)handle;
    register uint64 a2 asm ("a2") = (uint64)start_routine;
    register uint64 a3 asm ("a3") = (uint64)arg;

    // a4 je po standardu pokazivač na alociran stek.
    // Za sada ćemo proslediti 0 (nullptr) i pustiti kernel (TCB) da ga alocira.
    register uint64 a4 asm ("a4") = 0;

    __asm__ volatile ("ecall" : "+r" (a0):"r"(a1),"r" (a2),"r" (a3), "r" (a4) :"memory");
    return (int)a0;
}

int thread_exit()
{
    register uint64 a0 asm ("a0") = 0x12;
    __asm__ volatile ("ecall":"+r"(a0)::"memory");
    return (int)a0;
}

void thread_dispatch()
{
    register uint64 a0 asm ("a0") = 0x13;
    __asm__ volatile ("ecall":"+r"(a0)::"memory");
}
