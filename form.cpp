#include "form.h"
#include "ui_form.h"
#include "terminal_widget.h"
//#include "widget.h"
#include <QSplitter>
Form::Form(workspace ws, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
model = new QStandardItemModel(this);
//QSplitter* splitter;
//设置表头隐藏
ui->setupUi(this);
//connect(ui->Details, &QTextEdit:, this, &Form::wheelEvent);
terminal_widget *w = new terminal_widget();
ui->toolBox->removeItem(0);
ui->toolBox->addItem(w,"1");
ui->pushButton_run->hide();
form_ws = ws;
ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
menu = new QMenu(ui->treeView);
menu->addAction("运行");
menu->addAction("停止");
//splitter = new QSplitter(Qt::Vertical, this);
//splitter->addWidget(ui->treeView);
//splitter->addWidget(ui->Details);
//splitter->setStretchFactor(0,70);
//splitter->setStretchFactor(1,30);
connect(ui->treeView,&QTreeView::customContextMenuRequested,
        this,&Form::slotMenuPopup);
//ui->treeView->setHeaderHidden(true);

    //设置表头
    model->setHorizontalHeaderLabels(QStringList()<<"name"<<"description");

    //设置model
    //ui->treeView->setColumnWidth(0,500);
    ui->treeView->header()->setDefaultSectionSize(200);

    ui->treeView->setModel(model);
    //设置展开
    ui->treeView->expandAll();
    int num_pkg;

    //std::vector<QStandardItem*> pkg_items;  // 创建一个存储QStandardItem指针的向量_pkg
    for(int i = 0;; i++)
    {
        if(!ws.packages[i].name.empty())
        {
            //std::cout << ws.packages[i].nodes[0].run_command << std::endl;
            //std::cout << ws.packages[i].nodes[1].run_command << std::endl;
            // 创建一个新的QStandardItem作为package项
            QStandardItem *pkg_item = new QStandardItem(ws.packages[i].name.c_str());
            QStandardItem *node = new QStandardItem("nodes");
            QStandardItem *launch = new QStandardItem("launches");

            for(int j = 0;; j++)
            {
                // 检查是否存在node

                if(!ws.packages[i].nodes[j].name.empty())
                {
                    // 创建一个新的QStandardItem作为node项
                    QStandardItem *node_item = new QStandardItem(ws.packages[i].nodes[j].name.c_str());
                    node->appendRow(node_item);

                }
                else
                {
                    if(j==0)
                    {QStandardItem *node_item = new QStandardItem("None");
                    node->appendRow(node_item);}
                    break;
                }
            }
            for(int j = 0;; j++)
            {
                // 检查是否存在node

                if(!ws.packages[i].launches[j].name.empty())
                {
                    // 创建一个新的QStandardItem作为node项
                    QStandardItem *launch_item = new QStandardItem(ws.packages[i].launches[j].name.c_str());
                    //std::cout<<"123456"<<ws.packages[i].nodes[j].name<<std::endl;
                    // 将node项添加到package项下
                    //pkg_item->appendRow(node_item);
                    launch->appendRow(launch_item);

                }
                else
                {
                    if(j==0)
                    {
                    QStandardItem *launch_item = new QStandardItem("None");
                        launch->appendRow(launch_item);
                    }
                    break;
                }
            }
            // 将package项添加到模型中
            pkg_item->appendRow(launch);
            pkg_item->appendRow(node);
            QList<QStandardItem *> rowItems;
            QStandardItem *descriptionItem = new QStandardItem(ws.packages[i].description.c_str());
            rowItems <<pkg_item<<descriptionItem;
            model->appendRow(rowItems);
        }
        else
            break;
    }


}
void Form::slotMenuPopup(const QPoint &pos)
{
    menu->exec(ui->treeView->mapToGlobal(pos));
}
void Form::wheelEvent(QWheelEvent* event)
{
    QPoint globalMousePos = QCursor::pos();
    // 获取窗口的屏幕坐标
    QRect windowRect = ui->Details->frameGeometry();
    if (windowRect.contains(globalMousePos))
    {

        if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
            int delta = 0;
            #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            delta = event->angleDelta().y();
            #else
            delta = event->angleDelta().y();
            #endif
            if (delta > 0) {
                ui->Details->zoomIn();
            } else if (delta < 0) {
                ui->Details->zoomOut();
            }
            event->accept();
        } else {
            QWidget::wheelEvent(event);
        }
    }
}
Form::~Form()
{
    delete ui;
}
void Form::show_Details(workspace ws,std::string choose_type,int pkg,int nl)
{
    ui->Details->clear();
    ui->pushButton_run->setHidden(true);
    if(choose_type=="package")
    {
//        ui->Details->append("Type : Package");
//        ws.packages[pkg].name = "Name : " + ws.packages[pkg].name;
//        ws.packages[pkg].description = "Description : " + ws.packages[pkg].description;
//        ui->Details->append(ws.packages[pkg].name.c_str());
//        ui->Details->append(ws.packages[pkg].description.c_str());
        ui->Details->append("Type : package");
        ws.packages[pkg].name = "Name : " + ws.packages[pkg].name;
        ws.packages[pkg].description = "Description : " + ws.packages[pkg].description;
        ws.packages[pkg].version = "Version : " + std::string(ws.packages[pkg].version);
        ws.packages[pkg].maintainer = "Maintainer : " + std::string(ws.packages[pkg].maintainer);
        ws.packages[pkg].maintainer_email = "Maintainer Email : " + std::string(ws.packages[pkg].maintainer_email);
        ws.packages[pkg].path = "Path : " + ws.packages[pkg].path;
        ws.packages[pkg].license = "License : " + ws.packages[pkg].license;

        ui->Details->append(ws.packages[pkg].name.c_str());
        ui->Details->append(ws.packages[pkg].description.c_str());
        ui->Details->append(ws.packages[pkg].version.c_str());
        ui->Details->append(ws.packages[pkg].maintainer.c_str());
        ui->Details->append(ws.packages[pkg].maintainer_email.c_str());
        ui->Details->append(ws.packages[pkg].path.c_str());
        ui->Details->append(ws.packages[pkg].license.c_str());

        std::string execDependsString = "Execution Depends : ";
        //std::string execDependsString = "Execution Depends : ";
        for (const auto& depend : ws.packages[pkg].exec_depends) {
            execDependsString += depend + ", ";
        }
        execDependsString = execDependsString.substr(0, execDependsString.length() - 2); // 去除最后一个逗号和空格
        ui->Details->append(execDependsString.c_str());
    }
    else if(choose_type=="node")
    {
        ui->Details->append("Unable to find : Please to choose a node or launch file");
    }
    else if(choose_type=="launch")
    {
        ui->Details->append("Unable to find : Please to choose a node or launch file");
    }
    else if(choose_type=="node_child")
    {
        if(!ws.packages[pkg].nodes[nl].name.empty())
        {
            ui->Details->append("Type : Node");
            ws.packages[pkg].nodes[nl].name = "Name : " +ws.packages[pkg].nodes[nl].name;
            ws.packages[pkg].nodes[nl].run_command = "Quick run command : " + ws.packages[pkg].nodes[nl].run_command;
            ui->Details->append(ws.packages[pkg].nodes[nl].name.c_str());
            ui->Details->append(ws.packages[pkg].nodes[nl].run_command.c_str());
            ui->pushButton_run->setText("Run");
            ui->pushButton_run->setHidden(false);
        }
        else
        ui->Details->append("A node or a launch lile that does not exist, please choose a useful node of a launch file");
    }
    else if(choose_type=="launch_child")
    {
        if(!ws.packages[pkg].launches[nl].name.empty())
        {
        ui->Details->append("Type : launch file");
        ws.packages[pkg].launches[nl].name = "Name : " + ws.packages[pkg].launches[nl].name;
        ws.packages[pkg].launches[nl].path = "Path : " + ws.packages[pkg].launches[nl].path;
        ws.packages[pkg].launches[nl].launch_command = "Quick launch command : " + ws.packages[pkg].launches[nl].launch_command;

        ui->Details->append(ws.packages[pkg].launches[nl].name.c_str());
        ui->Details->append(ws.packages[pkg].launches[nl].path.c_str());
        ui->Details->append(ws.packages[pkg].launches[nl].launch_command.c_str());
        ui->pushButton_run->setText("Launch");
        ui->pushButton_run->setHidden(false);
        }
        else
        ui->Details->append("A node or a launch lile that does not exist, please choose a useful node of a launch file");
    }
    else{
        ui->Details->append("Unable to find");
    }
}
void Form::on_treeView_clicked(const QModelIndex &index)
{
    QStandardItem *item = model->itemFromIndex(index);

    // 初始化层级位置为 -1
    //int i = -1, j = -1, k = -1;
    int i = 0,j=0,k=0;
    if (item) {
        // 被点击项所在的行
        QStandardItem *parent_item = item->parent();
        if(parent_item){
            QStandardItem *grandparent_item = parent_item->parent();
            j=parent_item->row();
            if(grandparent_item){
                k=grandparent_item->row();
                if(parent_item->text().toStdString()=="nodes")
                {
                    choose_type="node_child";
                }
                else if(parent_item->text().toStdString()=="launches")
                {
                    choose_type="launch_child";
                }
                else
                {
                    perror("Unable to know what you chose");
                }
            }
            else
            {

                if(item->text().toStdString()=="nodes")
                {
                    choose_type="node";
                }
                else if(item->text().toStdString()=="launches")
                {
                    choose_type="launch";
                }
                else
                {
                    perror("ERROR");
                }
            }
            //show_Details(form_ws,choose_type,i);
        }
        else{
            choose_type = "package";


        }
        i = item->row();
        if(choose_type == "package")
        show_Details(form_ws,choose_type,i);
        else if(choose_type== "launch_child" || choose_type=="node_child")
        show_Details(form_ws,choose_type,k,i);
        else if(choose_type=="launch"||"node")
        show_Details(form_ws,choose_type,0);
        else
        {}
    }
}



