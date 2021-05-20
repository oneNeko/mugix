#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
public:
    Config();
    ~Config();

    void parse_config(int agrc, char *argv[]);

    //端口号
    int PORT;
    //指定文件目录
    std::string DIR_PATH;
};
#endif