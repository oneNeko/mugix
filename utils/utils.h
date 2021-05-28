#ifndef UTILS_H
#define UTILS_H
#endif

#include <string>
#include <vector>
#include <sys/epoll.h>

std::vector<std::string> SplitString(std::string source, std::string pattern);

bool IsFileExists(std::string path);
bool IsFileRead(std::string path);
bool IsFileWrite(std::string path);
bool IsFileExecute(std::string path);

std::string ReadFile(std::string path);

std::string UrlDecode(const std::string str);

class Utils
{
public:
    static void ModifyFd(int epoll_fd, int fd, int event);
};