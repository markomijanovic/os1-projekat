#include "../h/print.hpp"
#include "../h/workers.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp" // OBAVEZNO: Sada koristimo sistemske pozive za niti!

#include "../h/_console.hpp"

sem_t semafor_zavrsetka;

void idleBody(void* arg) {
    while (true) {
        thread_dispatch(); // Uvek prepuštaj procesor drugima čim stignu!
    }
}

int main()
{
    // 3. Postavljamo prekidnu rutinu
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // 1. KLJUČNA IZMENA: Memorija se MORA prva inicijalizovati!
    MemoryAllocator::initialize_memory();

    // 2. TEK SADA možemo da bezbedno zovemo "new" za bafere
    _console::init();



    printString("Testiranje C API alokatora zapoceto...\n");
    int* niz = new int[100];
    if (niz != nullptr) {
        printString("Uspesna ecall alokacija!\n");
        niz[0] = 42;
    } else {
        printString("Alokacija nije uspela!\n");
    }
    delete[] niz;
    printString("Testiranje zavrseno uspesno!\n");
    printString("Stres-test memorije zapocet...\n");

    // 1. Pokušavamo da alociramo ogroman komad memorije (npr. 10000 celih brojeva)
    int* ogroman_niz1 = new int[10000];
    if (ogroman_niz1 != nullptr) {
        printString("Prva velika alokacija: USPESNA\n");
    }

    // 2. Oslobađamo tu memoriju
    delete[] ogroman_niz1;
    printString("Memorija oslobodjena.\n");

    // 3. Pokušavamo ponovo da alociramo ISTU toliku kolicinu
    int* ogroman_niz2 = new int[10000];
    if (ogroman_niz2 != nullptr) {
        printString("Druga velika alokacija: USPESNA (Dealokacija radi savrseno!)\n");
    } else {
        printString("Druga alokacija: PALA (Greska u mem_free!)\n");
    }

    delete[] ogroman_niz2;
    printString("Testiranje zavrseno uspesno!\n");

    // --- KREIRANJE NITI I SINHRONIZACIJE ---
    sem_open(&semafor_zavrsetka, 0);

    // Povećaj niz na 7 mesta (main, 4 radnika, idle nit i konzolna nit)
    thread_t threads[7];

    thread_create(&threads[0], nullptr, nullptr); // Main nit
    thread_create(&threads[1], workerBodyA, nullptr);
    thread_create(&threads[2], workerBodyB, nullptr);
    thread_create(&threads[3], workerBodyC, nullptr);
    thread_create(&threads[4], workerBodyD, nullptr);

    // Kreiramo sistemske pozadinske niti
    thread_create(&threads[5], idleBody, nullptr);
    thread_create(&threads[6], _console::tx_thread, nullptr);

    // PALIMO PREKIDE TAJMERA
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    // MAIN ČEKA
    printString("Main nit trazi 4 resursa odjednom (wait_n)...\n");
    sem_wait_n(semafor_zavrsetka, 4);

    printString("Main nit uspesno sakupila sva 4 signala!\n");
    printString("Finished\n");

    return 0;
}