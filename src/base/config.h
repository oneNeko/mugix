#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
private:
    Config();

public:
    static Config GetConfig()
    {
        static Config instance;
        return instance;
    }

public:
    std::string ip_;
    int port_;
    std::string root_path_;
};

#endif