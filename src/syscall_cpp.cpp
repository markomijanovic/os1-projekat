#include "../h/syscall_cpp.hpp"
#include  "../h/MemoryAllocator.hpp"
// --- OPERATORI ZA MEMORIJU ---
void *operator new(size_t n) {
    return MemoryAllocator::mem_alloc(MemoryAllocator::size_to_blocks(n));
}

void *operator new[](size_t n) {
    return MemoryAllocator::mem_alloc(MemoryAllocator::size_to_blocks(n));
}

void operator delete(void *p) noexcept {
    MemoryAllocator::mem_free(p);
}

void operator delete[](void *p) noexcept {
    MemoryAllocator::mem_free(p);
}

// --- KLASA THREAD ---
Thread::Thread(void (*body)(void *), void *arg) : myHandle(nullptr), body(body), arg(arg) {
}

Thread::Thread() : myHandle(nullptr), body(nullptr), arg(nullptr) {
}

void Thread::wrapper(void *_thread) {
    if (_thread) {
        ((Thread *) _thread)->run();
    }
}

int Thread::start() {
    if (body) {
        return thread_create(&myHandle, body, arg);
    }
    return thread_create(&myHandle, wrapper, this);
}

//void Thread::join() {}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

Thread::~Thread() {
}

// --- KLASA SEMAPHORE ---
Semaphore::Semaphore(unsigned init) : myHandle(nullptr) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    if (myHandle) {
        sem_close(myHandle);
        myHandle = nullptr;
    }
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

// --- KLASA CONSOLE ---
char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
