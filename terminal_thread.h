#ifndef TERMINAL_THREAD_H
#define TERMINAL_THREAD_H
#include <QObject>
#include <QThread>
#include <functional>
class terminal_thread: public QThread
{
    Q_OBJECT
public:
    explicit terminal_thread(QObject *parent = nullptr) : QThread(parent) {}
    void addFunc(std::function<void()> func);
signals:
    void isDone();
protected:
    void run();
    std::function<void()> lambda;
};

#endif // TERMINAL_THREAD_H
