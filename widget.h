#ifndef WIDGET_H
#define WIDGET_H
#include <QVector>
#include <QWidget>
#include "ros2manager.h"
#include <QTextBrowser>
#include "findthread.h"
#include <QDebug>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    //void doLongRunningTask();

    std::string default_xml_path = "/home/ansel/ros2cotroller/params/profile.xml";
    void init();
    void appendToTextBrowser(const QString &result);
    void dealThread();
    void dealThread_auto();
    void dealDone_auto();
    void dealDone();
    void stopThread_auto();
    void stopThread();
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_refresh_clicked();
    void on_auto_find_clicked();
    void on_tabWidget_tabBarClicked(int index);

public slots:
    void updateOutputSlot(QString text);
    void addTabSlot(workspace workspace);
    void onTreeViewClicked(const QModelIndex &index) {
        // 处理点击事件
         //<< "Item clicked:" << index.data().toString;
    }

signals:
    void publishResult(const QString &result);
    void updateOutputText(QString text);
    void addTabSignal(workspace workspace);

private:
    std::vector<workspace> workspaces;
    ros2manager ros2_manager;
    Ui::Widget *ui;
    FindThread *thread;
    int choose_ws;

};
class CommandOutputHandler : public ICommandOutputCallback
{
public:
    CommandOutputHandler(QTextBrowser* textBrowser) : textBrowser(textBrowser) {}

    void onCommandOutput(const std::string& output) override {
        // 使用 Qt 的方法在 QTextBrowser 中显示输出
        QMetaObject::invokeMethod(textBrowser, "append", Qt::QueuedConnection,
                                  Q_ARG(QString, QString::fromStdString(output)));
    }

private:
    QTextBrowser* textBrowser;
};




#endif // WIDGET_H
