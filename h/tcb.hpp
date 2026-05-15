//
// Created by student on 14. 5. 2026..
//

#ifndef PROJECT_BASE_V1_1_CCB_HPP
#define PROJECT_BASE_V1_1_CCB_HPP
#include "scheduler.hpp"
#include "../lib/hw.h"

//thread control block
class TCB
{
public:
    ~TCB()
    {
        delete[] stack;
    }

    bool isFinished() const
    {
        return finished;
    }

    void setFinished(bool f)
    {
        finished = f;
    }

    uint64 getTimeSlice() const  { return timeSlice; }
    using Body = void (*)();

    static TCB *createThread(Body body);

    static void yield();

    static TCB *running;

private:
    TCB(Body body, uint64 timeSlice) : body(body),
                                       stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
                                       context({(uint64) &threadWrapper,
                                           stack != nullptr ? (uint64) &stack[STACK_SIZE] : 0}),
                                       timeSlice(timeSlice),
                                       finished(false)
    {
        if (body != nullptr)
        {
            Scheduler::put(this);
        }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = 1024;
    static uint64 constexpr TIME_SLICE = 2;
};
#endif //PROJECT_BASE_V1_1_CCB_HPP
