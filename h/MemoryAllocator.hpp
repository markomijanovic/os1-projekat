//
// Created by student on 8. 6. 2026..
//

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator
{
public:
    // Inicijalizacija alokatora (poziva se jednom pri pokretanju sistema)
    static void initialize_memory();

    // Alocira memoriju. Parametar size je broj BLOKOVA koje korisnik traži.
    // Vraća pokazivač na alocirani prostor (preskačući zaglavlje).
    static void *mem_alloc(size_t size);

    // Oslobađa memoriju na koju pokazuje ptr.
    static int mem_free(void *ptr);

    // Pomoćna funkcija koja pretvara bajtove u blokove uz zaokruživanje na gore.
    static int size_to_blocks(size_t size);


private:
    //Memory control block
    struct MCB
    {
        uint64 size; // Veličina ovog bloka izražena u blokovima (uključuje i samo zaglavlje)
        MCB *next;
        MCB *prev;
        bool isFree;
        MCB *left; // Pokazivač na FIZIČKI PRETHODNI segment u memoriji (nevezano da li je slobodan)
    };

    // Pokazivač na početak liste slobodnih blokova
    static MCB *free;


    // Granice heap prostora zaokružene na cele blokove radi lakšeg rada
    static uint64 heap_start_aligned;
    static uint64 heap_end_aligned;
    //
    static MCB *freeMCBs_join(MCB *left, MCB *right);
};
#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
