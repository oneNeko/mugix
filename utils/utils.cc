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

unsigned char ToHex(unsigned char x)
{
    return x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z')
        y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z')
        y = x - 'a' + 10;
    else if (x >= '0' && x <= '9')
        y = x - '0';
    //else assert(0);
    return y;
}

std::string UrlEncode(const std::string &str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

std::string UrlDecode(const std::string str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+')
            strTemp += ' ';
        else if (str[i] == '%')
        {
            //assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else
            strTemp += str[i];
    }
    return strTemp;
}