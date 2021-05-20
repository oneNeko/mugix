#include <cstdlib>
#include <unistd.h>

#include "config.h"

Config::Config() : PORT(50001)
{
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
            DIR_PATH=optarg;
            break;
        }
        }
    }
}