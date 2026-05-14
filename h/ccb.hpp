//
// Created by student on 14. 5. 2026..
//

#ifndef PROJECT_BASE_V1_1_CCB_HPP
#define PROJECT_BASE_V1_1_CCB_HPP
#include "scheduler.hpp"
#include "../lib/hw.h"

class CCB {
public:
    ~CCB(){delete[] stack;}

    bool isFinished() const { return finished; }

    void setFinished(bool f) { finished = f; }

    using Body = void (*)();

    static CCB *createCoroutine(Body body);

    static void yield();

    static CCB *running;

private:
    CCB(Body body) : body(body),
                     stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
                     context({(uint64) body, stack!=nullptr ? (uint64) &stack[STACK_SIZE] : 0}),
                     finished(false) {
        if(body!=nullptr){Scheduler::put(this);}
    }

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    bool finished;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 constexpr STACK_SIZE = 1024;
};
#endif //PROJECT_BASE_V1_1_CCB_HPP
