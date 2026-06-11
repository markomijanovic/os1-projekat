//
// Created by student on 11. 6. 2026..
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_CPP_HPP
#define PROJECT_BASE_V1_1_SYSCALL_CPP_HPP
#include "../h/syscall_c.hpp"

//1. preklapanje globalnih operatora za memoriju

void* operator new(size_t n);
void* operator new[](size_t n);
void operator delete(void* p) noexcept;
void operator delete[](void* p) noexcept;

//2.klasa thread
class Thread
{
public:
    Thread(void(*body)(void *),void* arg);
    virtual ~Thread();

    int start();
    void join();
    static void dispatch();
    static int sleep(time_t);
protected:
    Thread(); // Konstruktor za izvedene klase
    virtual void run() {} // Metoda koju korisnik preklapa
private:
    thread_t myHandle;
    void (*body)(void *);
    void* arg;

    // Statička funkcija koja sluzi da pozove virtualnu run() metodu
    static void wrapper(void* _thread);
};

//3.klasa semaphore
class Semaphore
{
public:
    Semaphore(unsigned int = 1);
    virtual ~Semaphore();

    int wait();
    int signal();
private:
    sem_t myHandle;
};

//4.klasa Console
class Console
{
public:
    static char getc();
    static void putc(char);
};


#endif //PROJECT_BASE_V1_1_SYSCALL_CPP_HPP
