#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/tcb.hpp"
#include "../h/MemoryAllocator.hpp" // OBAVEZNO: Uključujemo naš alokator memorije

void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

// Funkcija sada stoji SAMA (nije Riscv::), uzima parametre i vraca vrednost
extern "C" uint64 handleSupervisorTrap(uint64 a0, uint64 a1, uint64 a2, uint64 a3, uint64 a4)
{
    uint64 scause = Riscv::r_scause();
    uint64 retVal = a0;

    if (scause == 0x0000000000000009UL || scause == 0x0000000000000008UL)
    {
        // VOLATILE je apsolutno KLJUČAN ovde da nas promena konteksta ne bi uništila!
        volatile uint64 sepc = Riscv::r_sepc() + 4;
        volatile uint64 sstatus = Riscv::r_sstatus();

        if (a0 == 0x01) {
            //mem alloc
            retVal = (uint64)MemoryAllocator::mem_alloc((size_t)a1);
        }
        else if (a0 == 0x02) {
            //mem free
            retVal = (uint64)MemoryAllocator::mem_free((void*)a1);
        }
        else if (a0 == 0x11)
        {
            // thread_create
            TCB** handle = (TCB**)a1;
            *handle = TCB::createThread((TCB::Body)a2, (void*)a3);
            // --- OVO JE KLJUČNA POPRAVKA ---
            // Ako je start_routine (a2) nula, to je naša main nit!
            // Postavljamo je kao trenutno tekuću (running) nit.
            if (a2 == 0 && TCB::running == nullptr) {
                TCB::running = *handle;
            }
            if (*handle != nullptr) {
                retVal = 0; // Uspeh
            } else {
                retVal = -1; // Greška
            }
        }
        else if (a0 == 0x12)
        {
            // thread_exit
            TCB::running->setFinished(true);
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }
        else if (a0 == 0x13)
        {
            // thread_dispatch
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }

        Riscv::w_sstatus(sstatus);
        Riscv::w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        Riscv::mc_sip(Riscv::SIP_SSIP); // Prvo cistimo prekid!
        TCB::timeSliceCounter++;

        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            // VOLATILE je apsolutno KLJUČAN i ovde!
            volatile uint64 sepc = Riscv::r_sepc();
            volatile uint64 sstatus = Riscv::r_sstatus();

            TCB::timeSliceCounter = 0;
            TCB::dispatch();

            Riscv::w_sstatus(sstatus);
            Riscv::w_sepc(sepc);
        }
    }
    else if (scause == 0x8000000000000009UL)
    {
        console_handler();
    }

    return retVal;
}
