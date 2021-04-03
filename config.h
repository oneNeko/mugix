#ifndef CONFIG_H
#define CONFIG_H
class Config
{
public:
    Config();
    ~Config();

    void parse_config(int agrc, char *argv[]);

    //端口号
    int PORT;
};
#endif