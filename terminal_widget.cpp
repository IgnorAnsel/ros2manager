#include "terminal_widget.h"
#include "ui_terminal_widget.h"
//#include "form.h"
#include <iostream>
terminal_widget::terminal_widget(launch l,node n,std::string choose_type,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::terminal_widget)
{
    ui->setupUi(this);
    std::string com;
    if(choose_type == "launch_child")
    {
        std::cout << l.launch_command <<std::endl;
        com = "bash -c '" + l.launch_command + "'";
    }
    else if(choose_type == "node_child")
    {
        std::cout << n.run_command <<std::endl;
        com = "bash -c '" + n.run_command + "'";
    }

    if(!com.empty())
    {
        FILE *pipe = popen(com.c_str(), "r");
        //system(command);
        if (!pipe)
        {
            std::cerr << "无法打开管道。" << std::endl;
                return;
        }
        char buffer[128];
        std::string result = "";
        std::string error = ""; // 错误信息
        // 从管道中读取命令的输出和错误信息
        while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            result += buffer;
        }

        // 读取错误信息
        FILE *errorPipe = popen("echo $?", "r");
        if (errorPipe)
        {
            char errorBuffer[128];
            if (fgets(errorBuffer, sizeof(errorBuffer), errorPipe) != NULL)
            {
                error = errorBuffer;
            }
            pclose(errorPipe);
        }

        // 关闭管道
        pclose(pipe);
        ui->plainTextEdit->appendPlainText(result.c_str());
    }
    else{}
}

terminal_widget::~terminal_widget()
{
    delete ui;
}
void terminal_widget::on_lineEdit_returnPressed()
{
    QString codes = ui->lineEdit->text();
    QString path = QCoreApplication::applicationDirPath();
    std::string com = "bash -c '" + codes.toStdString() + "'";
    const char *command = com.c_str();
    std::cout << "执行命令: " << command << std::endl;
    // 打开管道并执行命令
    FILE *pipe = popen(command, "r");
    //system(command);
    if (!pipe)
    {
        std::cerr << "无法打开管道。" << std::endl;
            return;
    }

    char buffer[128];
    std::string result = "";
    std::string error = ""; // 错误信息

    // 从管道中读取命令的输出和错误信息
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        result += buffer;
    }

    // 读取错误信息
    FILE *errorPipe = popen("echo $?", "r");
    if (errorPipe)
    {
        char errorBuffer[128];
        if (fgets(errorBuffer, sizeof(errorBuffer), errorPipe) != NULL)
        {
            error = errorBuffer;
        }
        pclose(errorPipe);
    }

    // 关闭管道
    pclose(pipe);

    if (result.empty())
        result = "None";

    std::string out = path.toStdString() + " > " + codes.toStdString();
    ui->plainTextEdit->appendPlainText(out.c_str());

    ui->lineEdit->setText("");
    std::cout << result <<std::endl;

    // 输出错误信息
    if (result=="None")
    {
        ui->plainTextEdit->appendPlainText("ERROR : The command was not found or was not complete");
        std::cerr << "命令执行错误: " << error << std::endl;
    }
    else
    {
        ui->plainTextEdit->appendPlainText(result.c_str());
    }
}


