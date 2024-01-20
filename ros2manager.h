#ifndef ROS2MANAGER_H
#define ROS2MANAGER_H
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <tinyxml2.h>
#include <filesystem>
#include <thread>
//#include "widget.h"
#include <QWidget>
#define MAX_NODE 100
#define MAX_PACKAGE 100
#define MAX_LAUNCH 100
struct node
{
    std::string name;        // 节点名
    const char *type;        // 节点类型
    std::string run_command;  // 运行命令
};
struct launch
{
    std::string name;
    std::string path;
    std::string launch_command;//启动命令
};

struct package
{
    std::string name;//包名
    std::string version;//版本
    std::string description;//描述
    std::string maintainer;//维护者名字
    std::string maintainer_email;//维护者邮件
    std::string path;//包的地址
    std::string license;//证书
    std::vector<std::string> exec_depends;
    std::vector<std::string> test_depends;
    node nodes[MAX_NODE]; // 包中的节点
    launch launches[MAX_LAUNCH]; // 包中的launch文件
};
struct workspace
{
    std::string id;   // workspace id
    std::string path; // workspce path
    std::string name; // workspace name
    std::string command_to_ws;
    std::string command_source_install;
    std::string description;
    std::string author;
    std::string version;
    package packages[MAX_PACKAGE]; // workspace中的包
};
//struct workspace
//{
//    const char *id;   // workspace id
//    const char *path; // workspce path
//    const char *name; // workspace name
//    std::string command_to_ws;
//    std::string command_source_install;
//    const char *description;
//    const char *author;
//    const char *version;
//    package packages[MAX_PACKAGE]; // workspace中的包
//};
class ICommandOutputCallback
{
public:
    virtual ~ICommandOutputCallback() {}
    virtual void onCommandOutput(const std::string& output) = 0;
};
class ros2manager:public QObject
{
Q_OBJECT
private:
    // tinyxml2::XMLDocument doc;
    std::string load_command;                 // 加载最初始的命令行命令
    const char *profile_xml_file;             // 配置文件的地址
    std::string ros2_command;                 // ros2的命令行命令
    std::vector<workspace> workspaces;        // 容器
    std::string result_text;
    ICommandOutputCallback* callback;
signals:
    void addTabSignal(workspace workspace);
public:
    bool auto_find();
    std::string setCommandOutputCallback(ICommandOutputCallback* callback) {
        this->callback = callback;
        return get_output();
    }
    std::string get_output()
    {
        return result_text;
    }
    bool init(const char *profile_xml_file);
    std::string removeSpaces(std::string str);
    bool matching_str(std::string str1, std::string str2);
    void executeCMD(const char *cmd, char *result);
    std::string command_pulisher(std::string com);      // 发布命令
    void pkg_from_ws(std::string ws_path, int i); // 从工作空间中获取包
    package loadPackageXML(std::string file_path, package pkg);
    node node_from_pkg(int i, int j); // 从包中获取节点
    launch launch_from_pkg(int i, int j); // 从包中获取launch文件
    std::string get_path()
    {
        return profile_xml_file;
    }
    std::string removeString(std::string str, std::string str1);
    std::vector<workspace> get_workspace()
    {
        return workspaces;
    }
    ros2manager(/* args */);
    ~ros2manager();
};



#endif
