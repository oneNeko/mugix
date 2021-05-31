#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "utils.h"

std::vector<std::string> Utils::SplitString(std::string source, std::string pattern)
{
    int left = 0, right = 0;
    int pattern_size = pattern.length();
    std::vector<std::string> result;
    while (true)
    {
        right = int(source.find(pattern, left));
        if (right == -1)
        {
            result.push_back(source.substr(left));
            break;
        }
        result.push_back(source.substr(left, right - left));
        left = right + pattern_size;
    }
    return result;
}

// 添加epoll事件
void Utils::AddEvent(int epollfd, int fd, int state)
{
    struct epoll_event event;
    event.events = state;
    event.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}

// 删除epoll事件
void Utils::DeleteEvent(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

// 修改epoll事件
void Utils::ModifyEvent(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

std::string Utils::ReadFile(std::string path)
{
    std::ifstream infile;
    std::stringstream buffer;

    infile.open(path);

    buffer << infile.rdbuf();
    std::string contents(buffer.str());

    infile.close();

    return contents;
}