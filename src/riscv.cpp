#include "../h/riscv.hpp"

#include "../h/BoundedBuffer.hpp"
#include "../lib/console.h"
#include "../h/tcb.hpp"
#include "../h/MemoryAllocator.hpp" // OBAVEZNO: Uključujemo naš alokator memorije
#include "../h/_sem.hpp"
#include "../h/_console.hpp"
#include "../lib/hw.h"



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


        // Gasi SPP bit, osiguravajući da procesor padne u User mod nakon sret
        sstatus &= ~(1ULL << 8);

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
        else if (a0 == 0x21)
        {
            //sem_open
            _sem** handle = (_sem**)a1;
            *handle =new _sem((unsigned int) a2);
            if (*handle != nullptr) retVal = 0;
            else retVal = -1;
        }
        else if (a0 == 0x22)
        {
            //sem_close
            _sem* handle = (_sem*)a1;
            delete handle;
            retVal = 0;
        }
        else if (a0 == 0x23)
        {
            //sem_wait
            _sem* handle = (_sem*)a1;
            if (handle != nullptr) retVal = handle->wait();
            else retVal = -1;
        }
        else if (a0 == 0x24)
        {
            //sem_signal
            _sem* handle = (_sem*)a1;
            if (handle != nullptr) retVal = handle->signal();
            else retVal = -1;
        }
        else if (a0==0x25)
        {
            //sem_wait_n
            _sem * handle = (_sem*) a1;
            unsigned n=(unsigned)a2;
            if (handle != nullptr) retVal = handle->wait_n(n);
            else retVal = -1;
        }
        else if (a0 == 0x26)
        {
            //sem_signal_n
            _sem* handle = (_sem*)a1;
            unsigned n=(unsigned)a2;
            if (handle != nullptr) retVal = handle->signal_n(n);
            else retVal = -1;
        }
        else if (a0==0x31)
        {
            time_t ticks=(time_t)a1;
            retVal=TCB::sleep(ticks);
        }
        else if (a0 == 0x41)
        {
            //getc
            retVal = (uint64) _console::getc();
        }
        else if (a0 == 0x42)
        {
            // putc
            _console::putc((char)a1);
        }

        Riscv::w_sstatus(sstatus);
        Riscv::w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        TCB::updateSleepingThreads();
        Riscv::mc_sip(Riscv::SIP_SSIP); // Prvo cistimo prekid!
        TCB::timeSliceCounter++;

        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            // VOLATILE je apsolutno KLJUČAN i ovde!
            volatile uint64 sepc = Riscv::r_sepc();
            volatile uint64 sstatus = Riscv::r_sstatus();

            // Gasi SPP bit, osiguravajući pad u User mod čak i kada nas tajmer prekine
            sstatus &= ~(1ULL << 8);

            TCB::timeSliceCounter = 0;
            TCB::dispatch();

            Riscv::w_sstatus(sstatus);
            Riscv::w_sepc(sepc);
        }
    }
    else if (scause == 0x8000000000000009UL)
    {
        _console::handler();
    }

    return retVal;
}
