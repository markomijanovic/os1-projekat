#include "../h/print.hpp"
#include "../h/workers.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp" // OBAVEZNO: Sada koristimo sistemske pozive za niti!

// NAPOMENA: OBRISALI SMO #include "../h/tcb.hpp" !

int main() {
    MemoryAllocator::initialize_memory();
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

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

    // --- KORISTIMO C API ZA KREIRANJE NITI ---
    thread_t threads[5]; // Tip thread_t je naš TCB* (definisan u syscall_c.hpp)

    // Kreiramo glavnu nit (main) i radnike, prosledjujemo nullptr za arg
    thread_create(&threads[0], nullptr, nullptr);

    // TCB::running=threads[0] se sada radi automatski u prekidnoj rutini ili Scheduleru!
    // Ako se prevodilac bude bunio ovde oko setFinished, videcemo kako da to resimo bez TCB-a

    thread_create(&threads[1], workerBodyA, nullptr);
    printString("Thread A created\n");

    thread_create(&threads[2], workerBodyB, nullptr);
    printString("Thread B created\n");

    thread_create(&threads[3], workerBodyC, nullptr);
    printString("Thread C created\n");

    thread_create(&threads[4], workerBodyD, nullptr);
    printString("Thread D created\n");

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    // Pošto više ne vidimo TCB::isFinished(), za sada ćemo u main-u pozivati
    // thread_dispatch u jednostavnoj for petlji dok svi radnici ne zavrse.
    // Pravo rešenje za ovo čeka u Zadatku 3 (Semafori)!
    for (int i = 0; i < 50; i++) {
        thread_dispatch();
    }

    // Niti cistimo na kraju (pošto ne vidimo TCB, ovo ce pozvati delete iznutra
    // ako smo to dobro definisali, ili cemo ih samo ostaviti za sada dok ne sredimo
    // memorijski menadzment niti).

    printString("Finished\n");
    return 0;
}