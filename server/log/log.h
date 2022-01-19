#ifndef LOG_H
#define LOG_H

#include <string>
#include <mutex>

enum
{
    OFF,
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    ALL
};

class Log
{

private:
    Log();
    ~Log();
    bool GetTime(std::string &date, std::string &full_time);
    bool UpdateFp();
    void Out(int level, const char *strFormat, va_list arglist);

public:
    static Log *GetInstance()
    {
        static Log *instance;
        if (instance == NULL)
        {
            instance = new Log();
        }
        return instance;
    }

    bool Init(int log_level, std::string dir_path);

    // 方便书写使用小写
    void fatal(const char *format, ...);
    void error(const char *format, ...);
    void warn(const char *format, ...);
    void info(const char *format, ...);
    void debug(const char *format, ...);

    void flush(void);

private:
    int log_write_level_ = ALL;
    std::string dir_path_ = "/var/log/mugix/";
    std::string log_file_path_;

    FILE *log_fp_;
    std::string date_;

    std::mutex log_mutex_;
};

#endif