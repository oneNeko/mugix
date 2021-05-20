#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "utils.h"

std::vector<std::string> SplitString(std::string source, std::string pattern)
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

bool IsFileExists(std::string path)
{
    int res = access(path.c_str(), F_OK);
    if (res == 0)
    {
        return true;
    }
    return false;
}

bool IsFileRead(std::string path)
{
    int res = access(path.c_str(), R_OK);
    if (res == 0)
    {
        return true;
    }
    return false;
}

bool IsFileWrite(std::string path)
{
    int res = access(path.c_str(), W_OK);
    if (res == 0)
    {
        return true;
    }
    return false;
}

bool IsFileExecute(std::string path)
{
    int res = access(path.c_str(), X_OK);
    if (res == 0)
    {
        return true;
    }
    return false;
}

std::string ReadFile(std::string path)
{
    std::ifstream infile;
    std::stringstream buffer;

    infile.open(path);

    buffer << infile.rdbuf();
    std::string contents(buffer.str());

    infile.close();

    return contents;
}