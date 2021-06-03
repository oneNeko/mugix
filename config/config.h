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

    //epoll触发模式
    int epoll_trige_mode_ = 0;

    //日志模式
    int log_setting_ = 0;

    std::map<std::string, std::string> content_type_;
};
#endif