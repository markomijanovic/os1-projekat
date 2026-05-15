//
// Created by student on 14. 5. 2026..
//
#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/tcb.hpp"

void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause= r_scause();
    if (scause==0x0000000000000009UL)
    {
        //interrupt: no, cause code:environment call form S-mode (9)
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus= r_sstatus();
        TCB::timeSliceCounter= 0;
        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);


    }else if (scause ==0x8000000000000001UL)
    {
        //interupt: yes,cause code: supervisor softvare interupt(timer)
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter>=TCB::running->getTimeSlice())
        {
            uint64 sepc = r_sepc();
            uint64 sstatus= r_sstatus();
            TCB::timeSliceCounter= 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }


        mc_sip(SIP_SSIP);

    }else if (scause ==0x8000000000000009UL)
    {
        //interupt: yes,cause code: supervisor external interupt(console)
        console_handler();
    }else
    {
        //unexpected trap cause
        //print(scause)
        //print(sepc)
        //print(stval)
    }
}
