#include <string>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"

using std::string;

Log::Log()
{
    
}

Log::~Log()
{
    if (log_fp_ != NULL)
    {
        fclose(log_fp_);
    }
}

// 初始化日志配置
// log_level 日志记录级别
// dir_path 日志地址
bool Log::Init(int log_level, string dir_path)
{
    log_write_level_ = log_level;
    dir_path_ = dir_path;

    if (!UpdateFp())
    {
        return false;
    }

    return true;
}

// 获取日期和完整时间
bool Log::GetTime(string &date, string &full_time)
{
    char buf[256], buf1[256];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    strftime(buf, sizeof(buf) - 1, "%Y-%m-%d", localtime(&tv.tv_sec));
    strftime(buf1, sizeof(buf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
    date = buf;
    full_time = buf1;

    return true;
}

bool Log::UpdateFp()
{
    if (access(dir_path_.c_str(), F_OK) != 0)
    {
        if (mkdir(dir_path_.c_str(), 777) != 0)
        {
            int n = errno;
            return false;
        }
    }

    string tmp;
    GetTime(date_, tmp);
    string path = dir_path_ + date_ + ".log";
    log_fp_ = fopen(path.c_str(), "a");
    if (log_fp_ == nullptr)
    {
        printf("open log file failed.");
        return false;
    }
    return true;
}

// 日志输出主接口
// 日志级别
// 日志内容
void Log::Out(int level, const char *format, va_list arglist)
{
    string log = "";

    string date, time;
    GetTime(date, time);
    log += time;

    switch (level)
    {
    case FATAL:
    {
        log += " [FATAL] ";
        break;
    }
    case ERROR:
    {
        log += " [ERROR] ";
        break;
    }
    case WARN:
    {
        log += " [WARN] ";
        break;
    }
    case INFO:
    {
        log += " [INFO] ";
        break;
    }
    case DEBUG:
    {
        log += " [DEBUG] ";
        break;
    }
    }

    char strResult[2048] = {0};
    vsprintf(strResult, format, arglist);
    log += strResult;
    log += "\r\n";

    if (date != date_)
    {
        date_ = date;
        if (!UpdateFp())
        {
            return;
        }
    }

    log_mutex_.lock();

    int res = fputs(log.c_str(), log_fp_);
    res = fflush(log_fp_);
    log_mutex_.unlock();
}

void Log::fatal(const char *format, ...)
{
    if (log_write_level_ >= FATAL)
    {
        va_list valst;
        va_start(valst, format);
        Out(FATAL, format, valst);
        va_end(valst);
    }
}

void Log::error(const char *format, ...)
{
    if (log_write_level_ >= ERROR)
    {
        va_list valst;
        va_start(valst, format);
        Out(ERROR, format, valst);
        va_end(valst);
    }
}

void Log::warn(const char *format, ...)
{
    if (log_write_level_ >= WARN)
    {
        va_list valst;
        va_start(valst, format);
        Out(WARN, format, valst);
        va_end(valst);
    }
}

void Log::info(const char *format, ...)
{
    if (log_write_level_ >= INFO)
    {
        va_list valst;
        va_start(valst, format);
        Out(INFO, format, valst);
        va_end(valst);
    }
}

void Log::debug(const char *format, ...)
{
    if (log_write_level_ >= DEBUG)
    {
        va_list valst;
        va_start(valst, format);
        Out(DEBUG, format, valst);
        va_end(valst);
    }
}
