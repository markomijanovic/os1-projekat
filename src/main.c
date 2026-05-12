//
// Created by student on 12. 5. 2026..
//
#include "../lib/console.h"
#include "../lib/hw.h"

uint64 pc;

void a() {
    __asm__ volatile("mv %0, ra":"=r" (pc));
}

void b() {
    __asm__ volatile("mv ra, %0"::"r" (pc));
}
void main() {
    a();
    __putc('a');
    __putc('\n');
    b();
}