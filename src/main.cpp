#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/workers.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.hpp" // Dodato zaglavlje

int main() {
    // 1. OBAVEZNO inicijalizujemo memoriju na samom početku!
    MemoryAllocator::initialize_memory();

    // --- POCETAK TESTA ALOKATORA ---
    printString("Testiranje alokatora zapoceto...\n");

    // Tražimo memoriju za 3 različita objekta (pretvaramo bajtove u blokove)
    size_t blocks1 = MemoryAllocator::size_to_blocks(100); // Npr. tražimo 100 bajtova
    size_t blocks2 = MemoryAllocator::size_to_blocks(250);
    size_t blocks3 = MemoryAllocator::size_to_blocks(50);

    // Alociramo
    void* p1 = MemoryAllocator::mem_alloc(blocks1);
    void* p2 = MemoryAllocator::mem_alloc(blocks2);
    void* p3 = MemoryAllocator::mem_alloc(blocks3);

    // Nateraj prevodilac da evaluira pokazivace
    printString("p1 adresa: "); printInteger((uint64)p1); printString("\n");
    printString("p2 adresa: "); printInteger((uint64)p2); printString("\n");
    printString("p3 adresa: "); printInteger((uint64)p3); printString("\n");

    if (p1 != nullptr && p2 != nullptr && p3 != nullptr) {
        printString("Uspesna alokacija sva 3 bloka!\n");

        // Probamo upis u alociranu memoriju da vidimo da li puca (Page Fault)
        char* text = (char*)p1;
        text[0] = 'O'; text[1] = 'K'; text[2] = '\n'; text[3] = '\0';
        printString(text);
    }

    // Sada testiramo dealokaciju i čuveno spajanje blokova!

    // Oslobađamo srednji blok (Slučaj 4 iz našeg koda - nema slobodnih komšija)
    MemoryAllocator::mem_free(p2);

    // Oslobađamo levi blok (Slučaj 2 - desni komšija (p2) je slobodan, spajaju se!)
    MemoryAllocator::mem_free(p1);

    // Oslobađamo desni blok (Slučaj 3 - levi komšija (p1+p2) je slobodan, sve se spaja u jedan veliki heap!)
    MemoryAllocator::mem_free(p3);

    printString("Testiranje alokatora zavrseno.\n");
    // --- KRAJ TESTA ALOKATORA ---

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