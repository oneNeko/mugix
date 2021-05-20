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