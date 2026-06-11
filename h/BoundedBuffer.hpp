//
// Created by student on 11. 6. 2026..
//

#ifndef PROJECT_BASE_V1_1_BOUNDEDBUFFER_HPP
#define PROJECT_BASE_V1_1_BOUNDEDBUFFER_HPP

#include "_sem.hpp"
class BoundedBuffer
{
public:
    BoundedBuffer();
    ~BoundedBuffer();

    void put(char c);
    char get();
private:
    static const int CAPACITY = 256;
    char buffer[CAPACITY];
    int head;
    int tail;

    _sem * spaceAvailable;
    _sem * itemAvailable;
    _sem * mutex;

};


#endif //PROJECT_BASE_V1_1_BOUNDEDBUFFER_HPP
