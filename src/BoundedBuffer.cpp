//
// Created by student on 11. 6. 2026..
//

#include "../h/BoundedBuffer.hpp"

BoundedBuffer::BoundedBuffer()
{
    head=0;
    tail=0;
    count=0;

    spaceAvailable = new _sem(CAPACITY);
    itemAvailable = new _sem(0);
    mutex = new _sem(1);
}

BoundedBuffer::~BoundedBuffer()
{
    delete mutex;
    delete itemAvailable;
    delete spaceAvailable;
}

void BoundedBuffer::put(char c)
{
    spaceAvailable->wait();
    mutex->wait();

    buffer[tail] = c;
    tail= (tail+1)%CAPACITY;
    count++;

    mutex->signal();
    itemAvailable->signal();
}

bool BoundedBuffer::tryPut(char c)
{
    if (count>=CAPACITY)
    {
        return false;
    }
    buffer[tail]=c;
    tail=(tail+1)%CAPACITY;
    count++;

    itemAvailable->signal();
    return true;
}

char BoundedBuffer::get()
{
    itemAvailable->wait();
    mutex->wait();

    char c = buffer[head];
    head = (head+1)%CAPACITY;
    count--;

    mutex->signal();
    spaceAvailable->signal();
    return c;
}
