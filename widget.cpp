#include "widget.h"
#include "./ui_widget.h"
#include "ros2manager.h"
#include "form.h"
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #include <QDesktopWidget>
#else
    #include <QScreen>
#endif
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 获取屏幕的大小和分辨率
    int screenWidth;
    int screenHeight;

    this->setWindowTitle("ROS2 Manager");
    this->resize(QSize(1024,720));
    // 计算窗口的位置
    int windowWidth = this->width();
    int windowHeight = this->height();
    this->setStyleSheet("background-color:white");
    this->setStyleSheet("color:black");


    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QDesktopWidget* desktop = QApplication::desktop();
        screenWidth = desktop->width();
        screenHeight = desktop->height();
    #else
        QScreen* screen = QGuiApplication::primaryScreen();
        // 获取屏幕的几何尺寸
        QRect screenGeometry = screen->geometry();
        screenWidth = screenGeometry.width();
        screenHeight = screenGeometry.height();
    #endif
    // 将窗口移动到计算出的位置
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
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
        //std::cout << "进入" << std::endl;
        workspaces.clear();
        if(ros2_manager.auto_find())
        {
            emit updateOutputText(QString::fromStdString(ros2_manager.get_output()));
            workspaces = ros2_manager.get_workspace();
            for(int i = 0; i < workspaces.size(); i++)
            {
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
    //std::cout << "处理" << std::endl;
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

