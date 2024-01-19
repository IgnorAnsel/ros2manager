#include "findthread.h"

void FindThread::run()
{
    lambda();
    emit isDone();
    emit isDone_auto();
}

void FindThread::addFunc(std::function<void()> func)
{
    lambda=func;
}
