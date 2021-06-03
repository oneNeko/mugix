#include <cstdlib>
#include <unistd.h>

#include "config.h"
#include "../utils/utils.h"

Config::Config() : PORT(50001)
{
    auto data = Utils::ReadFile("conf/mime.conf");
    auto data_set = Utils::SplitString(data, ";\n");

    for (auto iter : data_set)
    {
        auto data_line = Utils::SplitString(iter, " : ");
        if(data_line.size()!=2){
            continue;
        }
        
        auto mimes = Utils::SplitString(data_line[1], " , ");
        for (auto it : mimes)
        {
            content_type_[it] = data_line[0];
        }
    }
}

Config::~Config()
{
}

void Config::ParseConfig(int argc, char *argv[])
{
    int opt;
    const char *str = "p:d:m:l:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 'p':
        {
            PORT = atoi(optarg);
            break;
        }
        case 'd':
        {
            DIR_PATH = optarg;
            break;
        }
        case 'm':
        {
            epoll_trige_mode_ = atoi(optarg);
            break;
        }
        case 'l':
        {
            log_setting_ = atoi(optarg);
            break;
        }
        }
    }
}

Config *Config::GetInstance()
{
    static Config *instance;
    if (instance == NULL)
    {
        instance = new Config();
    }
    return instance;
}