#ifndef TERMINAL_WIDGET_H
#define TERMINAL_WIDGET_H

#include <QWidget>
#include <QProcess>
namespace Ui {
class terminal_widget;
}

class terminal_widget : public QWidget
{
    Q_OBJECT

public:
    explicit terminal_widget(QWidget *parent = nullptr);
    ~terminal_widget();

private slots:
    void on_lineEdit_returnPressed();

private:
    Ui::terminal_widget *ui;
};

#endif // TERMINAL_WIDGET_H
