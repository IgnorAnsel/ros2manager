#ifndef TERMINAL_WIDGET_H
#define TERMINAL_WIDGET_H

#include <QWidget>
#include <QProcess>
#include "ros2manager.h"
#include "terminal_thread.h"
namespace Ui {
class terminal_widget;
}

class terminal_widget : public QWidget
{
    Q_OBJECT
public:
    explicit terminal_widget(launch ll,node nn,std::string choose_type_,QWidget *parent = nullptr);
    //void updateUI(QString result, QString error);
    void init(launch l,node n,std::string choose_type);
    void dealThread();
    void dealDone();
    void stopThread();
    ~terminal_widget();
signals:
    void addbuffer(char *buffer);
    void resultAvailable(QString result, QString error);
private slots:
    void on_lineEdit_returnPressed();
public slots:
    void addbufferto(char *buffer);
private:
    node n;
    launch l;
    std::string choose_type;
    terminal_thread *thread;
    Ui::terminal_widget *ui;
};

#endif // TERMINAL_WIDGET_H
