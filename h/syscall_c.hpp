//
// Created by student on 10. 6. 2026..
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_HPP
#define PROJECT_BASE_V1_1_SYSCALL_C_HPP

#include "../lib/hw.h"


void * mem_alloc(size_t size);
int mem_free(void * p);

class TCB;
typedef TCB* thread_t ;

int thread_create(thread_t* handle,void(*start_routine)(void*),void* arg);
int thread_exit();
void thread_dispatch();

class _sem;
typedef _sem* sem_t ;

int sem_open(sem_t* handle,unsigned int init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);
int sem_wait_n(sem_t id, unsigned int n);
int sem_signal_n(sem_t id, unsigned int n);

int time_sleep(time_t ticks);

char getc();

void putc(char c);

#endif //PROJECT_BASE_V1_1_SYSCALL_C_HPP
