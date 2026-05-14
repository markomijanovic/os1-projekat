#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include "list.hpp"


class CCB;

class Scheduler {
private:
    static List<CCB> readyCoroutineQueue;

public:
    static CCB *get();

    static void put(CCB *ccb);
};

#endif
