#include "ros2manager.h"

std::string ros2manager::command_pulisher(std::string com)
{
    com = "bash -c '" + com + "'";
    const char *command = com.c_str();
    //std::cout << "执行命令: " << command << std::endl;
    system(command);
    FILE *pipe = popen(command, "r");
    if (!pipe)
    {
        std::cerr << "无法打开管道。" << std::endl;
            return NULL;
    }
    char buffer[128];
    std::string result = "";

    // 从管道中读取命令的输出
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        result += buffer;
    }
    if (result.empty());
        //result = "None";
    // 关闭管道
    pclose(pipe);

    // 打印命令的输出
    // std::cout << "命令的输出:\n"
    //           << result << std::endl;
    result_text = result;
    if (callback) {
        callback->onCommandOutput(result);
    }
    return result;
}
bool ros2manager::auto_find()
{
    workspaces.clear();
    std::string findpath = "/home";
    if (std::filesystem::exists(findpath) && std::filesystem::is_directory(findpath))
    {

        int j=0;

        for (const auto &entry : std::filesystem::recursive_directory_iterator(findpath))
        {
            if (entry.path().filename() == "package.xml")
            {

                if (entry.path().string().find("/install/") != std::string::npos)
                    continue;
                if  (entry.path().string().find("/Src/") != std::string::npos||entry.path().string().find(".local") != std::string::npos)
                    continue;
                workspace ws;
                ws.path = entry.path().c_str();
                // std::cout << "找到ros2包: " << ws.path << std::endl;
                size_t src_pos = entry.path().string().find("/src/");
                if (src_pos != std::string::npos)
                {
                    size_t slash1_pos = entry.path().string().rfind("/", src_pos - 1);
                    if (slash1_pos != std::string::npos)
                    {
                        size_t slash2_pos = entry.path().string().find("/", src_pos - 5);
                        if (slash2_pos != std::string::npos)
                        {
                            bool islike = 0;
                            std::string between_slashes = entry.path().string().substr(slash1_pos + 1, slash2_pos - slash1_pos - 1);
                            std::string ws_path = entry.path().string().substr(0, src_pos);
                            for(int i=0;i<workspaces.size();i++)
                            {
                                if(ws_path == workspaces[i].path)
                                {
                                    islike = 1;
                                    break;
                                }
                            }
                            if(islike==0)
                            {
                                std::cout << "ws_path: " << ws_path << std::endl;
                                ws.id = std::to_string(j);
                                ws.name = between_slashes;
                                ws.path = ws_path;
                                ws.description = "ros2 workspace";
                                ws.version = std::to_string(0);
                                ws.command_to_ws = "cd " + ws_path;
                                ws.command_source_install = "source " + ws_path + "/install/setup.sh";
                                workspaces.push_back(ws);
                                std::cout << j << std::endl;
                                j++;
                            }
                            //std::cout << "ws.name: " << workspaces[0].name << std::endl;
                        }
                    }
                }

            }
            // std::cout<< workspaces.size() <<std::endl;
        }
        for (size_t i = 0; i < workspaces.size(); ++i)
        {
            std::cout << "<---" << workspaces[i].name << "--->          " << std::endl;
            pkg_from_ws(workspaces[i].path, i);
            emit addTabSignal(workspaces[i]);
            //std::cout << "000000000000000000" << workspaces[i].packages[0].nodes[0].name << std::endl;
        }
        return true;
    }
    else
    {
        std::cerr << "Failed to load XML file" <<std::endl;
        //perror("Failed to load XML file");
        return false;
    }
}
bool ros2manager::init(const char *profile_xml_file)
{
    // std::cout << "ros2manager init" << std::endl;
    workspaces.clear();
    tinyxml2::XMLDocument doc;
    doc.LoadFile(profile_xml_file);
    // std::cout << command_pulisher("/opt/ros/humble/bin/ros2 pkg list") << std::endl;
    //  检查XML文件是否加载成功
    if (doc.LoadFile(profile_xml_file) == tinyxml2::XML_SUCCESS)
    {
        // 解析XML文件
        std::cout << "Loaded XML file: " << profile_xml_file << std::endl;
        tinyxml2::XMLElement *workspaceElement = doc.FirstChildElement("WorkSpace")->FirstChildElement("workspace");
        while (workspaceElement)
        {
            const char *idAttribute = workspaceElement->Attribute("id");
            const char *name = workspaceElement->FirstChildElement("name")->GetText();
            const char *path = workspaceElement->FirstChildElement("path")->GetText();
            const char *description = workspaceElement->FirstChildElement("description")->GetText();
            const char *version = workspaceElement->FirstChildElement("version")->GetText();

            if (idAttribute && name && path && description && version) // 输出各个读的值
            {
                // std::cout << "ID: " << idAttribute << std::endl;
                // std::cout << "Name: " << name << std::endl;
                // std::cout << "Path: " << path << std::endl;
                // std::cout << "Description: " << description << std::endl;
                // std::cout << "Version: " << version << std::endl;
            }
            workspace ws;
            ws.id = idAttribute;
            ws.name = name;
            ws.path = path;
            ws.description = description;
            ws.version = version;
            ws.command_to_ws = "cd " + (std::string)path;
            ws.command_source_install = "source " + (std::string)path + "/install/setup.sh";
            workspaces.push_back(ws);
            workspaceElement = workspaceElement->NextSiblingElement("workspace");
        }
        for (size_t i = 0; i < workspaces.size(); ++i)
        {
            //std::cout << "<---" << workspaces[i].name << "--->          " << std::endl;
            pkg_from_ws(workspaces[i].path, i);
            emit addTabSignal(workspaces[i]);
            //std::cout << "000000000000000000" << workspaces[i].packages[0].nodes[0].name << std::endl;
        }

    }
    else
    {
        std::cerr << "Failed to load XML file" <<std::endl;
        //perror("Failed to load XML file");
        return false;
    }
    return true;
}

launch ros2manager::launch_from_pkg(int i, int j)
{
    launch l;
    std::string path = workspaces[i].packages[j].path;
    size_t Pos = path.find("package.xml");
    if (Pos != std::string::npos)
    {
        // 删除从该位置到字符串末尾的部分
        path.erase(Pos);
    }
    std::string launch_file = path + "launch/";
    path = path + "launch/";
    std::cout << "path: " << path << std::endl;
    int k = 0;
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
    {
        // 遍历给定目录
        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            auto filename = entry.path().filename().string(); // 获取文件名
            // 如果你只想要文件名而不是完整路径，请使用 filename().string()
            if (filename.find(".py")!=std::string::npos)
            {
                //std::cout << "wim      usaiudbiubsiyabyubdiubsaf" << filename << std::endl;
                workspaces[i].packages[j].launches[k].name = filename.c_str();
                path = path + filename;
                workspaces[i].packages[j].launches[k].launch_command = workspaces[i].command_source_install + ";ros2 launch " + workspaces[i].packages[j].name + " " +workspaces[i].packages[j].launches[k].name;
                workspaces[i].packages[j].launches[k].path = path.c_str();
                //std::cout << "===" << workspaces[i].packages[i].launches[j].name << std::endl;
                k++;
                path = launch_file;
            }


        }
    }
    else
    {
        std::cerr << "路径不存在或不是一个目录" << std::endl;
    }
    return l;

}

void ros2manager::pkg_from_ws(std::string ws_path, int i)
{
    ws_path = ws_path + "/src";
    std::cout << "ws_path: " << ws_path << std::endl;
    int j = 0;
    if (std::filesystem::exists(ws_path) && std::filesystem::is_directory(ws_path))
    {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(ws_path))
        {
            if (entry.path().filename() == "package.xml")
            {
                package pkg;
                pkg.path = entry.path().c_str();
                workspaces[i].packages[j].path = pkg.path;
                tinyxml2::XMLDocument doc;
                doc.LoadFile(workspaces[i].packages[j].path.c_str());
                tinyxml2::XMLElement *root = doc.FirstChildElement("package");
                if(root)
                {
                    pkg = loadPackageXML(workspaces[i].packages[j].path, pkg);
                }
                else continue;

                workspaces[i].packages[j] = pkg;
                launch_from_pkg(i, j);
                node_from_pkg(i, j);
                std::cout << "===package: " << workspaces[i].packages[j].launches[0].name << std::endl;
                j++;
            }
        }
    }
    else
    {
        std::cerr << "Error: " << ws_path << " is not a valid directory" << std::endl;

    }
}
std::string ros2manager::removeSpaces(std::string str)
{
    std::string new_str;
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] != ' ')
        {
            new_str.push_back(str[i]);
        }
    }
    return new_str;
}

std::string ros2manager::removeString(std::string str, std::string str1)
{
    str.erase(0, str1.length() + 1);
    return str;
}

bool ros2manager::matching_str(std::string str_1, std::string str_2)
{
    str_1 = removeSpaces(str_1);
    str_2 = removeSpaces(str_2);
    return (str_1.compare(str_2) == 0);
}

node ros2manager::node_from_pkg(int i, int j)
{
    node n;
    //std::string commad;
    //commad = workspaces[i].command_source_install + ";" + "ros2 pkg executables" + " | grep " + workspaces[i].packages[j].name;
    //std::string str = command_pulisher(commad);
    size_t startPos = 0;
    int node_index = 0;
    std::string str = workspaces[i].packages[j].path;
    size_t srcPos = str.find("src");
    str = str.substr(0, srcPos);
    str = str +"build/" + workspaces[i].packages[j].name;

    //std::string str = command_pulisher(commad);
    //std::cout << "=================="<<str << "   "  << std::endl;
    std::string directory = str;
    if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (std::filesystem::is_regular_file(entry.status())) {
// 对于Windows系统，检查文件扩展名是否为.exe
#ifdef _WIN32
                if (entry.path().extension() == ".exe") {
                    std::cout << entry.path() << std::endl;
                }
#else
                // 对于Unix-like系统，检查文件是否具有执行权限
                if ((entry.status().permissions() & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) {
                    if(entry.path().string().find(".so")== std::string::npos&&entry.path().string().find(".out")== std::string::npos&&
                        entry.path().string().find(".a")== std::string::npos&&entry.path().string().find(".lib")==std::string::npos&&
                        entry.path().string().find(".bin")== std::string::npos&&entry.path().string().find(".built_by")==std::string::npos&&
                        entry.path().string().find(".json")==std::string::npos&&
                        entry.path().string().find(".last")==std::string::npos&&
                        entry.path().string().find(".txt")==std::string::npos&&
                        entry.path().string().find(".o")==std::string::npos&&
                        entry.path().string().find(".make")==std::string::npos&&
                        entry.path().string().find(".ts")==std::string::npos&&
                        entry.path().string().find(".ini")==std::string::npos&&
                        entry.path().string().find(".sh")==std::string::npos&&
                        entry.path().string().find("marks")==std::string::npos
                        )
                    {
                        std::cout << "==========" <<entry.path().filename().string() << std::endl;
                        workspaces[i].packages[j].nodes[node_index].name = entry.path().filename().string();
                        workspaces[i].packages[j].nodes[node_index].run_command =workspaces[i].command_source_install +";ros2 run "+workspaces[i].packages[j].name+" "+workspaces[i].packages[j].nodes[node_index].name;
                        node_index++;
                    }
                }
#endif
            }
        }
    }
//    while (startPos != std::string::npos)
//    {
//        startPos = str.find(workspaces[i].packages[j].name, startPos);

//        if (startPos != std::string::npos)
//        {
//            // 找到下一个空格的位置
//            size_t spacePos = str.find(' ', startPos + 1);
//            size_t linePos = str.find('\n', startPos + 1);
//            if (spacePos != std::string::npos)
//            {
//                // 提取出的字符串节点名
//                std::string extractedString = str.substr(spacePos + 1, linePos - spacePos - 1);

//                workspaces[i].packages[j].nodes[node_index].name = extractedString;
//                std::cout << node_index <<std::endl;
//                workspaces[i].packages[j].nodes[node_index].run_command = workspaces[i].command_source_install + ";ros2 run " + workspaces[i].packages[j].name + " " + extractedString + " &";
//                std::cout << workspaces[i].packages[j].nodes[node_index].run_command <<std::endl;
//                // command_pulisher(ws.packages[i].nodes[node_index].run_command);
//                //  更新搜索起始位置
//                startPos = spacePos + 1;
//                node_index++;
//            }
//            else
//                break;
//        }
//        else
//            break;
//    }
    return n;
}

package ros2manager::loadPackageXML(std::string file_path, package pkg)
{
    std::cout << "Loading package.xml from " << file_path << std::endl;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(file_path.c_str());
    tinyxml2::XMLElement *root = doc.FirstChildElement("package");
    if (root)
    {
        std::cout << "you" <<std::endl;
        pkg.name = root->FirstChildElement("name")->GetText();
        pkg.version = root->FirstChildElement("version")->GetText();
        pkg.description = root->FirstChildElement("description")->GetText();
        pkg.maintainer = root->FirstChildElement("maintainer")->GetText();
        pkg.maintainer_email = root->FirstChildElement("maintainer")->Attribute("email");
        pkg.license = root->FirstChildElement("license")->GetText();
        std::cout << "    < " << pkg.name << std::endl;
        std::cout << removeString("ros2", "ros") << std::endl;
        for (tinyxml2::XMLElement *dep = root->FirstChildElement("exec_depend"); dep != nullptr; dep = dep->NextSiblingElement("exec_depend"))
        {
            pkg.exec_depends.push_back(dep->GetText());
        }
        for (tinyxml2::XMLElement *dep = root->FirstChildElement("test_depend"); dep != nullptr; dep = dep->NextSiblingElement("test_depend"))
        {
            pkg.test_depends.push_back(dep->GetText());
        }
    }
    else
    {
        std::cout << "no" <<std::endl;
    }

    return pkg;
}

ros2manager::ros2manager(/* args */)
{
}

ros2manager::~ros2manager()
{
}
