#include "../h/print.hpp"
#include "../h/workers.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp" // OBAVEZNO: Sada koristimo sistemske pozive za niti!

#include "../h/_console.hpp"
#include "../h/tcb.hpp"

sem_t semafor_zavrsetka;

void idleBody(void* arg) {
    while (true) {
        thread_dispatch(); // Uvek prepuštaj procesor drugima čim stignu!
    }
}

extern void userMain();

#ifndef RUN_INTERNAL_TESTS
#define RUN_INTERNAL_TESTS 0
#endif
int main()
{
    // 3. Postavljamo prekidnu rutinu
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // 1. KLJUČNA IZMENA: Memorija se MORA prva inicijalizovati!
    MemoryAllocator::initialize_memory();

    // 2. TEK SADA možemo da bezbedno zovemo "new" za bafere
    _console::init();


    thread_t mainThread = TCB::createThread(nullptr, nullptr, true);
    TCB::running = mainThread;
    TCB::createThread(idleBody, nullptr, true);
    TCB::createThread(_console::tx_thread, nullptr, true);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);




#if RUN_INTERNAL_TESTS
    // Ovde možeš kasnije vratiti svoje interne testove ako hoćeš.
#else
    userMain();
#endif

    return 0;
}
