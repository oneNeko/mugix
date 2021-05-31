#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

class Config
{
private:
    Config();

public:
    static Config *GetInstance();
    ~Config();

    void ParseConfig(int agrc, char *argv[]);

    //端口号
    int PORT;
    //指定文件目录
    std::string DIR_PATH;

    std::map<std::string, std::string> content_type_;
};
#endif