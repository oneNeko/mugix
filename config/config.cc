#include <cstdlib>
#include <unistd.h>

#include "config.h"
#include "../utils/utils.h"

Config::Config() : PORT(50001), DIR_PATH("../html/")
{
    auto data = ReadFile("conf/mime.conf");
    auto data_set = SplitString(data, ";\n");

    for (auto iter : data_set)
    {
        auto data_line = SplitString(iter, " : ");
        if(data_line.size()!=2){
            continue;
        }
        
        auto mimes = SplitString(data_line[1], " , ");
        for (auto it : mimes)
        {
            content_type[it] = data_line[0];
        }
    }
}

Config::~Config()
{
}

void Config::parse_config(int argc, char *argv[])
{
    int opt;
    const char *str = "p:d:";
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
        }
    }
}

Config *Config::get_instance()
{
    static Config *instance;
    if (instance == NULL)
    {
        instance = new Config();
    }
    return instance;
}