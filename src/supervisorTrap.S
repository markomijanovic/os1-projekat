.extern handleSupervisorTrap

.align 4 // zbog stvec registra da ima dve najmanje nule
.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv,@function
_ZN5Riscv14supervisorTrapEv:

    addi sp,sp,-256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index*8(sp)
    .endr

    # C handler prima argumente iz a0-a4.
    call handleSupervisorTrap

    # Povratna vrednost se vraća kroz sačuvani a0.
    sd a0, 10*8(sp)

    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index*8(sp)
    .endr
    addi sp,sp,256
    sret
