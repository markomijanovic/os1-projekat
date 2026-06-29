//
// Created by student on 11. 6. 2026..
//

#ifndef PROJECT_BASE_V1_1_CONSOLE_HPP
#define PROJECT_BASE_V1_1_CONSOLE_HPP

#include  "../h/BoundedBuffer.hpp"

class _console
{
public:
    static void init();
    static void putc(char c);
    static char getc();
    static void flush();
    static void handler();// Poziva se iz prekida (scause 9)
    static void tx_thread(void * arg);// Ovo je ona profina "Posebna nit jezgra"

private:
    static BoundedBuffer * rx_buffer;
    static BoundedBuffer * tx_buffer;

};

#endif //PROJECT_BASE_V1_1_CONSOLE_HPP
