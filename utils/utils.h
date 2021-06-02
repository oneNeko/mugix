#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <sys/epoll.h>

class Utils
{
public:
    // 切割字符串
    static std::vector<std::string> SplitString(std::string source, std::string pattern);

    // 修改epoll状态
    
    static void AddEvent(int epollfd, int fd, int state);
    static void DeleteEvent(int epollfd, int fd, int state);
    static void ModifyEvent(int epollfd, int fd, int state);

    static std::string ReadFile(std::string path);

    // 设置非阻塞
    static void SetNonblock(int fd);
};

#endif