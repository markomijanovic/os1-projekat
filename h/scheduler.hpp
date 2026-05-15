#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include "list.hpp"


class TCB;

class Scheduler {
private:
    static List<TCB> readyCoroutineQueue;

public:
    static TCB *get();

    static void put(TCB *ccb);
};

#endif
