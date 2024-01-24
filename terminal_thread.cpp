#include "terminal_thread.h"
void terminal_thread::run()
{
    lambda();
    emit isDone();
}

void terminal_thread::addFunc(std::function<void()> func)
{
    lambda=func;
}
