//
// Created by student on 14. 5. 2026..

//
#include "../lib/mem.h"

void *operator new(uint64 n) {
    return __mem_alloc(n);
};

void *operator new[](uint64 n) {
    return __mem_alloc(n);
};

void operator delete(void *p) noexcept {
    __mem_free(p);
};

void operator delete[](void *p) noexcept {
    __mem_free(p);
};
