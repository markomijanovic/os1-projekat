//
// Created by student on 11. 6. 2026..
//
#include "../h/_console.hpp"

#include "../h/syscall_c.hpp"
#include "../lib/hw.h"
#include "../h/tcb.hpp"

BoundedBuffer * _console::rx_buffer=0;
BoundedBuffer * _console::tx_buffer=0;

void _console::init()
{
    rx_buffer=new BoundedBuffer();
    tx_buffer=new BoundedBuffer();
}


// Poziva se iz sistemskog poziva 0x42
void _console::putc(char c)
{
    tx_buffer->put(c);
}

// Poziva se iz sistemskog poziva 0x41
char _console::getc()
{
    return rx_buffer->get();
}

//Poziva se iz PLIC hardverskog prekida
void _console::handler()
{
    uint64 irq = plic_claim();

    if (irq == CONSOLE_IRQ)
    {
        // Ako je neko kliknuo tastaturu, kupimo znakove i stavljamo u rx bafer
        while (*((volatile char*)CONSOLE_STATUS)& CONSOLE_RX_STATUS_BIT)
        {
            rx_buffer->tryPut(*((volatile char*)CONSOLE_RX_DATA));
        }
    }
    plic_complete(irq);
}

// "Posebna nit jezgra" koja jedina sme da komunicira sa ekranom!
void _console::tx_thread(void *arg)
{
    while (1)
    {
        char c=tx_buffer->get();// Spava dok nema slova u baferu

        // Ceka da hardver bude spreman, usput prepustajuci procesor (NEMA BLOKADE KERNELA)
        while (!(*((volatile char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT))
        {
            thread_dispatch();
        }
        *((volatile char*)CONSOLE_TX_DATA) = c;// Salje na ekran

    }
}
