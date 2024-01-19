#ifndef FINDTHREAD_H
#define FINDTHREAD_H
#include <QObject>
#include <QThread>
#include <functional>

class FindThread : public QThread
{
    Q_OBJECT
public:
    explicit FindThread(QObject *parent = nullptr) : QThread(parent) {}

    void addFunc(std::function<void()> func);
protected:
    //run() 是继承下来的虚函数
    // 是 protected 的函数
    //也是唯一的线程处理函数，注意是唯一
    //不能直接调用，正确姿势是通过start()间接调用
    void run();
    std::function<void()> lambda;


signals:
    void isDone(); //线程运行后发出结束信号，只声明不实现
    void isDone_auto();


public slots:

};

#endif // FINDTHREAD_H
