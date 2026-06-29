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
    bool tryPut(char c);
    char get();
    bool isEmpty() const;
private:
    static const int CAPACITY = 256;
    char buffer[CAPACITY];
    int head;
    int tail;
    int count;

    _sem * spaceAvailable;
    _sem * itemAvailable;
    _sem * mutex;

};


#endif //PROJECT_BASE_V1_1_BOUNDEDBUFFER_HPP
