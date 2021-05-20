#ifndef UTILS_H
#define UTILS_H
#endif

#include <string>
#include <vector>

std::vector<std::string> SplitString(std::string source, std::string pattern);

bool IsFileExists(std::string path);
bool IsFileRead(std::string path);
bool IsFileWrite(std::string path);
bool IsFileExecute(std::string path);

std::string ReadFile(std::string path);