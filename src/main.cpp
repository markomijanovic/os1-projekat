#include "../h/print.hpp"
#include "../h/workers.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp" // OBAVEZNO: Sada koristimo sistemske pozive za niti!
#include "../h/syscall_cpp.hpp"

#include "../h/_console.hpp"
#include "../h/tcb.hpp"

sem_t semafor_zavrsetka;

void idleBody(void* arg) {
    while (true) {
        thread_dispatch(); // Uvek prepuštaj procesor drugima čim stignu!
    }
}

extern void userMain();

#ifndef RUN_INTERNAL_TESTS
#define RUN_INTERNAL_TESTS 0
#endif

#if RUN_INTERNAL_TESTS
static int passedTests = 0;
static int failedTests = 0;

static void printUnsigned(uint64 value)
{
    char buffer[20];
    int count = 0;

    do {
        buffer[count++] = (char) ('0' + value % 10);
        value /= 10;
    } while (value != 0);

    while (count > 0) {
        putc(buffer[--count]);
    }
}

static void reportCheck(const char *name, bool ok)
{
    printString(ok ? "[OK] " : "[FAIL] ");
    printString(name);
    printString("\n");

    if (ok) {
        passedTests++;
    } else {
        failedTests++;
    }
}

static void testMemoryAllocator()
{
    printString("\n--- memory allocator stress ---\n");

    void *zero = mem_alloc(0);
    reportCheck("mem_alloc(0) returns nullptr", zero == nullptr);
    reportCheck("mem_free(nullptr) returns error", mem_free(nullptr) == -1);

    void *doubleFree = mem_alloc(96);
    reportCheck("mem_alloc small block", doubleFree != nullptr);
    reportCheck("mem_free allocated block", doubleFree && mem_free(doubleFree) == 0);
    reportCheck("double free returns error", doubleFree && mem_free(doubleFree) == -1);

    void *blocks[48];
    bool ok = true;
    for (int i = 0; i < 48; i++) {
        size_t size = (i % 9 + 1) * 37;
        blocks[i] = mem_alloc(size);
        if (!blocks[i]) {
            ok = false;
            continue;
        }

        char *bytes = (char *) blocks[i];
        bytes[0] = (char) i;
        bytes[size - 1] = (char) (i + 1);
    }
    reportCheck("48 mixed allocations", ok);

    ok = true;
    for (int i = 0; i < 48; i += 2) {
        if (blocks[i] && mem_free(blocks[i]) != 0) {
            ok = false;
        }
        blocks[i] = nullptr;
    }
    reportCheck("free every second block", ok);

    void *largeBlocks[16];
    ok = true;
    for (int i = 0; i < 16; i++) {
        largeBlocks[i] = mem_alloc(256 + i * 31);
        if (!largeBlocks[i]) {
            ok = false;
        }
    }
    reportCheck("allocate into fragmented heap", ok);

    ok = true;
    for (int i = 1; i < 48; i += 2) {
        if (blocks[i] && mem_free(blocks[i]) != 0) {
            ok = false;
        }
    }
    for (int i = 0; i < 16; i++) {
        if (largeBlocks[i] && mem_free(largeBlocks[i]) != 0) {
            ok = false;
        }
    }
    reportCheck("free all stress allocations", ok);
}

static void testSemaphoreApi()
{
    printString("\n--- semaphore api stress ---\n");

    reportCheck("sem_wait(nullptr) returns error", sem_wait(nullptr) == -1);
    reportCheck("sem_signal(nullptr) returns error", sem_signal(nullptr) == -1);
    reportCheck("sem_close(nullptr) returns error", sem_close(nullptr) == -1);

    sem_t sem = nullptr;
    bool ok = sem_open(&sem, 0) == 0 && sem != nullptr;
    reportCheck("sem_open init 0", ok);

    if (ok) {
        bool nOk = true;
        nOk = nOk && sem_wait_n(sem, 0) == 0;
        nOk = nOk && sem_signal_n(sem, 3) == 0;
        nOk = nOk && sem_wait_n(sem, 2) == 0;
        nOk = nOk && sem_wait(sem) == 0;
        nOk = nOk && sem_signal_n(sem, 0) == 0;
        reportCheck("sem_wait_n/sem_signal_n resource accounting", nOk);
        reportCheck("sem_close valid semaphore", sem_close(sem) == 0);
    }
}

struct CWorkerArg
{
    int loops;
    sem_t done;
    sem_t mutex;
    volatile int *counter;
};

static void cCounterWorker(void *arg)
{
    CWorkerArg *data = (CWorkerArg *) arg;
    for (int i = 0; i < data->loops; i++) {
        sem_wait(data->mutex);
        (*data->counter)++;
        sem_signal(data->mutex);

        if (i % 7 == 0) {
            thread_dispatch();
        }
    }
    sem_signal(data->done);
}

static void testCThreads()
{
    printString("\n--- c thread stress ---\n");

    const int threadCount = 6;
    const int loops = 40;
    volatile int counter = 0;
    sem_t done = nullptr;
    sem_t mutex = nullptr;
    thread_t threads[threadCount];
    CWorkerArg args[threadCount];

    bool ok = sem_open(&done, 0) == 0 && sem_open(&mutex, 1) == 0;
    reportCheck("c thread semaphores open", ok);
    if (!ok) {
        return;
    }

    ok = true;
    for (int i = 0; i < threadCount; i++) {
        args[i].loops = loops;
        args[i].done = done;
        args[i].mutex = mutex;
        args[i].counter = &counter;

        if (thread_create(&threads[i], cCounterWorker, &args[i]) != 0) {
            ok = false;
        }
    }
    reportCheck("create 6 c threads", ok);

    for (int i = 0; i < threadCount; i++) {
        sem_wait(done);
    }

    reportCheck("c threads exact shared counter", counter == threadCount * loops);

    sem_close(done);
    sem_close(mutex);
}

struct SleepArg
{
    int id;
    time_t ticks;
    sem_t done;
    sem_t mutex;
    volatile int *finishedCount;
    volatile int *order;
};

static void sleepWorker(void *arg)
{
    SleepArg *data = (SleepArg *) arg;
    time_sleep(data->ticks);

    sem_wait(data->mutex);
    int index = (*data->finishedCount)++;
    data->order[index] = data->id;
    sem_signal(data->mutex);

    sem_signal(data->done);
}

static void testSleepOrdering()
{
    printString("\n--- sleep ordering stress ---\n");

    volatile int finishedCount = 0;
    volatile int order[2] = {0, 0};
    sem_t done = nullptr;
    sem_t mutex = nullptr;
    thread_t threads[2];
    SleepArg args[2];

    bool ok = sem_open(&done, 0) == 0 && sem_open(&mutex, 1) == 0;
    reportCheck("sleep semaphores open", ok);
    if (!ok) {
        return;
    }

    args[0] = {1, 5, done, mutex, &finishedCount, order};
    args[1] = {2, 15, done, mutex, &finishedCount, order};

    ok = thread_create(&threads[0], sleepWorker, &args[0]) == 0;
    ok = ok && thread_create(&threads[1], sleepWorker, &args[1]) == 0;
    reportCheck("create sleeping threads", ok);

    sem_wait(done);
    sem_wait(done);

    reportCheck("shorter sleep finishes first", order[0] == 1 && order[1] == 2);

    sem_close(done);
    sem_close(mutex);
}

class CppCounterWorker : public Thread
{
public:
    CppCounterWorker(Semaphore *done, Semaphore *mutex, volatile int *counter, int loops)
        : Thread(), done(done), mutex(mutex), counter(counter), loops(loops) {}

protected:
    void run() override
    {
        for (int i = 0; i < loops; i++) {
            mutex->wait();
            (*counter)++;
            mutex->signal();

            if (i % 5 == 0) {
                Thread::dispatch();
            }
            if (i % 13 == 0) {
                Thread::sleep(1);
            }
        }
        done->signal();
    }

private:
    Semaphore *done;
    Semaphore *mutex;
    volatile int *counter;
    int loops;
};

static void testCppApi()
{
    printString("\n--- cpp api stress ---\n");

    const int threadCount = 5;
    const int loops = 35;
    volatile int counter = 0;

    Semaphore *done = new Semaphore(0);
    Semaphore *mutex = new Semaphore(1);
    CppCounterWorker *workers[threadCount];

    bool ok = done != nullptr && mutex != nullptr;
    reportCheck("cpp semaphores allocate", ok);
    if (!ok) {
        return;
    }

    ok = true;
    for (int i = 0; i < threadCount; i++) {
        workers[i] = new CppCounterWorker(done, mutex, &counter, loops);
        if (!workers[i] || workers[i]->start() != 0) {
            ok = false;
        }
    }
    reportCheck("create 5 cpp threads", ok);

    for (int i = 0; i < threadCount; i++) {
        done->wait();
    }

    reportCheck("cpp threads exact shared counter", counter == threadCount * loops);

    for (int i = 0; i < threadCount; i++) {
        delete workers[i];
    }
    delete mutex;
    delete done;

    ok = true;
    for (int i = 0; i < 24; i++) {
        Semaphore *sem = new Semaphore(1);
        if (!sem || sem->wait() != 0 || sem->signal() != 0) {
            ok = false;
        }
        delete sem;
    }
    reportCheck("cpp Semaphore destructor stress", ok);
}

static void testInvalidInputs()
{
    printString("\n--- invalid input checks ---\n");

    reportCheck("thread_create nullptr handle returns error",
                thread_create(nullptr, cCounterWorker, nullptr) == -1);
    reportCheck("time_sleep(0) returns immediately", time_sleep(0) == 0);
}

static void runInternalTests()
{
    printString("\n==============================\n");
    printString("INTERNAL KERNEL STRESS TESTS\n");
    printString("==============================\n");

    testInvalidInputs();
    testMemoryAllocator();
    testSemaphoreApi();
    testCThreads();
    testSleepOrdering();
    testCppApi();

    printString("\n==============================\n");
    printString("PASSED: ");
    printUnsigned((uint64) passedTests);
    printString("\nFAILED: ");
    printUnsigned((uint64) failedTests);
    printString("\n==============================\n");
}
#endif

int main()
{
    // 3. Postavljamo prekidnu rutinu
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // 1. KLJUČNA IZMENA: Memorija se MORA prva inicijalizovati!
    MemoryAllocator::initialize_memory();

    // 2. TEK SADA možemo da bezbedno zovemo "new" za bafere
    _console::init();


    thread_t mainThread = TCB::createThread(nullptr, nullptr, true);
    TCB::running = mainThread;
    TCB::createThread(idleBody, nullptr, true);
    TCB::createThread(_console::tx_thread, nullptr, true);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);




#if RUN_INTERNAL_TESTS
    runInternalTests();
#else
    userMain();
#endif

    _console::flush();

    return 0;
}
