#include "config.h"

#include <fstream>
#include <iostream>
#include <string>
#include <string.h>

#include "logger.h"

Config::Config()
{
    std::fstream config_stream;
    const char *config_path = "mugix.cfg";
    config_stream.open(config_path);
    if (!config_stream.is_open())
    {
        error("打开%s失败", config_path);
        return;
    }

    char line[1024];
    std::string key, value;
    while (!config_stream.eof())
    {
        memset(line, 0, 1024);
        config_stream.getline(line, 1024); 

        std::string str(line);
        int pos = str.find('=');
        if (pos != std::string::npos)
        {
            key = str.substr(0, pos);
            value = str.substr(pos + 1);
        }

        if (key == "ip")
        {
            ip_ = value;
        }
        else if (key == "port")
        {
            port_ = atoi(value.c_str());
        }
        else if (key == "root")
        {
            root_path_ = value;
        }
    }
}