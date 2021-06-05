#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "http_response.h"
#include <iostream>
#include <sys/stat.h>

#include "../config/config.h"

using namespace std;

void HttpResponse::GetResponseCode()
{
}

//Fri, 28 May 2021 06:25:21 GMT
string HttpResponse::GetDate()
{
    struct timeval tv;
    char buf[64];
    gettimeofday(&tv, NULL);
    strftime(buf, sizeof(buf) - 1, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&tv.tv_sec));

    return buf;
}

void HttpResponse::Process()
{
    CheckFile();
    SetType();
    date_ = GetDate();
    SetContentLength();
}

std::string HttpResponse::GetStatusCode(int status)
{
    std::string status_line = "";
    if (status >= HTTP_OK && status < HTTP_LAST_2XX)
    {
        status_line = http_status_lines[status - HTTP_OK];
    }
    else if (status > HTTP_LAST_2XX && status < HTTP_LAST_3XX)
    {
        status_line = http_status_lines[status + HTTP_OFF_3XX - 301];
    }
    else if (status > HTTP_LAST_3XX && status < HTTP_LAST_4XX)
    {
        status_line = http_status_lines[status + HTTP_OFF_4XX - 400];
    }
    else if (status > HTTP_LAST_4XX && status < HTTP_LAST_5XX)
    {
        status_line = http_status_lines[status + HTTP_OFF_5XX - 500];
    }
    return status_line;
}

string HttpResponse::GetHeader()
{
    /*
HTTP/1.1 302 FOUND
Server: nginx/1.18.0 (Ubuntu)
Date: Fri, 28 May 2021 06:25:21 GMT
Content-Type: text/html; charset=utf-8
Content-Length: 237
Location: https://fin.oneneko.com/login?next=%2F
Connection: keep-alive
Vary: Cookie
*/
    string header = protocol_ + " " + GetStatusCode(206) + "\r\n";
    header += "Server: " + server_ + "\r\n";
    header += "Date: " + date_ + "\r\n";
    header += "Content-Type: " + content_type_ + "\r\n";
    if (content_length_ != 0)
    {
        if (content_length_ > 100 * 1024)
        {
            header += "Content-Length: " + to_string(content_length_) + "\r\n";
            header += "Content-Range: bytes 0-" + to_string(100 * 1024) + "/" + to_string(content_length_) + "\r\n";
            content_length_ = 100 * 1024;
        }
    }
    header += "Connection: " + connection_ + "\r\n";
    header += "\r\n";

    return header;
}
void HttpResponse::CheckFile()
{
    if (file_path_ == "")
    {
        return;
    }

    auto config = Config::GetInstance();
    file_path_ = config->DIR_PATH + file_path_;

    //是否存在
    int res = access(file_path_.c_str(), F_OK);
    if (res != 0)
    {
        response_code_ = 404;
        return;
    }

    //是否可读
    res = access(file_path_.c_str(), R_OK);
    if (res != 0)
    {
        response_code_ = 403;
        return;
    }

    //获取属性
    struct stat st;
    stat(file_path_.c_str(), &st);

    if (S_ISDIR(st.st_mode))
    {
        response_code_ = 404;
        return;
    }

    response_code_ = 200;

    content_length_ = st.st_size;
    type_ = T_FILE;
}

void HttpResponse::SetContentLength()
{
    switch (type_)
    {
    case T_FILE:
    {
        break;
    }
    case T_CONTENT:
    {
        content_length_ = content_.length();
        break;
    }
    }
}

void HttpResponse::SetType()
{
    if (response_code_ < 100 || response_code_ >= 300)
    {
        if (type_ < 0)
        {
            type_ = T_CONTENT;
            content_ = to_string(response_code_);
        }
    }
}

void HttpResponse::Clear()
{
    response_code_ = 200;
    date_.clear();
    content_type_.clear();
    content_length_ = 0;
    connection_ = "keep-alive";

    type_ = -1;
    file_path_.clear();
    content_.clear();
}