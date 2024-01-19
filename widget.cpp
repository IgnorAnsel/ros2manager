#include "widget.h"
#include "./ui_widget.h"
#include "ros2manager.h"
#include "form.h"
#include <QDesktopWidget>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 获取屏幕的大小和分辨率
    QDesktopWidget* desktop = QApplication::desktop();
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    this->setWindowTitle("ROS2 Manager");
    this->resize(QSize(1024,720));
    // 计算窗口的位置
    int windowWidth = this->width();
    int windowHeight = this->height();
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    // 将窗口移动到计算出的位置


    this->move(x, y);
    thread = new FindThread(this);
    init();
    qRegisterMetaType<workspace>("workspace");
    connect(this, &Widget::updateOutputText, this, &Widget::updateOutputSlot);
    connect(this, &Widget::addTabSignal, this, &Widget::addTabSlot);
    connect(&ros2_manager, &ros2manager::addTabSignal, this, &Widget::addTabSlot);
//,Qt::DirectConnection
    CommandOutputHandler* handler = new CommandOutputHandler(ui->output);
    ros2_manager.setCommandOutputCallback(handler);
    //关联结束动作以及启动动作
    connect(ui->auto_find,&QPushButton::clicked, this , &Widget::dealThread_auto);
    connect(ui->refresh,&QPushButton::clicked, this , &Widget::dealThread);
    connect(thread, &FindThread::isDone, this, &Widget::dealDone);
    connect(thread, &FindThread::isDone_auto, this, &Widget::dealDone_auto);
}

Widget::~Widget()
{
    delete ui;
}
void Widget::init()
{
    ui->tabWidget->setTabText(0,"setting");
    ui->xml_path->setText(QString::fromStdString(Widget::default_xml_path));

    //ros2_manager.init(Widget::default_xml_path.c_str());

}
void Widget::appendToTextBrowser(const QString &result)
{
    ui->output->append(result);
}

void Widget::on_refresh_clicked()
{
    //std::vector <workspace>().swap(workspaces);
    ui->refresh->setText("Loading");
    QApplication::processEvents(); // 处理所有挂起的事件，包括界面更新
//    if(ros2_manager.init(Widget::default_xml_path.c_str()))
//    {
//        ui->output->append(ros2_manager.get_output().c_str());
//        //ui->refresh->setText("refresh");
//        workspaces = ros2_manager.get_workspace();
//        for(int i = 0;i<workspaces.size();i++)
//        {
//            int tabCount = ui->tabWidget->count();
//            bool hasSameName = false;
//            for (int j = 0; j < tabCount; ++j) {
//                QString tabName = ui->tabWidget->tabText(j);
//                if (tabName == (workspaces[i].name).c_str()) {
//                    hasSameName = true;
//                    break;
//                }
//            }
//            if (!hasSameName) {
//                Form *newtab = new Form(workspaces[i]);
//                ui->tabWidget->addTab(newtab, (workspaces[i].name).c_str());
//            }
//        }
//    }
//    else
//    {
//        ui->refresh->setText("ERROR");
//    }
}
void Widget::stopThread()
{
    thread->quit();
    thread->wait();
}
void Widget::stopThread_auto()
{
    thread->quit();
    thread->wait();
}
void Widget::dealDone(){
    std::cout<< "it is over"<<std::endl;
    ui->refresh->setText("refresh");
}
void Widget::dealDone_auto(){
    std::cout<< "it is over"<<std::endl;
    ui->auto_find->setText("auto");
}
void Widget::dealThread_auto()
{
    thread->addFunc([=](){
        std::vector <workspace>().swap(workspaces);
        if(ros2_manager.auto_find())
        {
            emit updateOutputText(QString::fromStdString(ros2_manager.get_output()));
            workspaces = ros2_manager.get_workspace();
            for(int i = 0; i < workspaces.size(); i++)
            {
                std::cout << "进入" << std::endl;
                        emit addTabSignal(workspaces[i]);
            }
        }
        else
        {
            emit updateOutputText("ERROR");
        }
    });
    thread->start();
}
void Widget::dealThread()
{

    thread->addFunc([=](){
        //std::vector<workspace>().swap(workspaces);
        if(ros2_manager.init(Widget::default_xml_path.c_str()))
        {
            emit updateOutputText(QString::fromStdString(ros2_manager.get_output()));
            workspaces = ros2_manager.get_workspace();
            for(int i = 0; i < workspaces.size(); i++)
            {
                std::cout << "进入" << std::endl;
                emit addTabSignal(workspaces[i]);
            }
        }
        else
        {
            emit updateOutputText("ERROR");
        }
    });
    thread->start();
}
void Widget::updateOutputSlot(QString text)
{
    ui->output->append(text);
}

void Widget::addTabSlot(workspace workspace)
{
    int tabCount = ui->tabWidget->count();
    bool hasSameName = false;
    for (int j = 0; j < tabCount; ++j) {
        QString tabName = ui->tabWidget->tabText(j);
        if (tabName == workspace.name.c_str()) {
            hasSameName = true;
            break;
        }
    }
    if (!hasSameName) {
        Form *newtab = new Form(workspace);
        ui->tabWidget->addTab(newtab, workspace.name.c_str());
    }
    std::cout << "处理" << std::endl;
}
void Widget::on_auto_find_clicked()
{
    std::vector <workspace>().swap(workspaces);
    ui->auto_find->setText("wait");
    QApplication::processEvents(); // 处理所有挂起的事件，包括界面更新
//        if(ros2_manager.auto_find())
//    {
//        ui->output->append(ros2_manager.get_output().c_str());
//        ui->refresh->setText("refresh");
//        workspaces = ros2_manager.get_workspace();
//        for(int i = 0;i<workspaces.size();i++)
//        {
//            int tabCount = ui->tabWidget->count();
//            bool hasSameName = false;
//            for (int j = 0; j < tabCount; ++j) {
//                QString tabName = ui->tabWidget->tabText(j);
//                if (tabName == (workspaces[i].name).c_str()) {
//                    hasSameName = true;
//                    break;
//                }
//            }
//            if (!hasSameName) {
//                Form *newtab = new Form(workspaces[i]);
//                ui->tabWidget->addTab(newtab, (workspaces[i].name).c_str());
//            }
//        }
//    }
//    else
//    {
//        ui->auto_find->setText("ERROR");
//    }
//    ui->auto_find->setText("auto");
}


void Widget::on_tabWidget_tabBarClicked(int index)
{
    choose_ws = index-1;
    std::cout << choose_ws <<std::endl;
}

