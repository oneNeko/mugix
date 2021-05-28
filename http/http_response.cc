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
    Date = GetDate();
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
    string header = protocol + " " + GetStatusCode(response_code) + "\r\n";
    header += "Server: " + Server + "\r\n";
    header += "Date: " + Date + "\r\n";
    header += "Content-Type: " + Content_Type + "\r\n";
    if (Content_Length != 0)
    {
        header += "Content-Length: " + to_string(Content_Length) + "\r\n";
    }
    header += "Connection: " + Connection + "\r\n";
    header += "\r\n";

    return header;
}
void HttpResponse::CheckFile()
{
    if (file_path == "")
    {
        return;
    }

    auto config = Config::get_instance();
    file_path = config->DIR_PATH + file_path;

    //是否存在
    int res = access(file_path.c_str(), F_OK);
    if (res != 0)
    {
        response_code = 404;
        return;
    }

    //是否可读
    res = access(file_path.c_str(), R_OK);
    if (res != 0)
    {
        response_code = 403;
        return;
    }

    //获取属性
    struct stat st;
    stat(file_path.c_str(), &st);

    if (S_ISDIR(st.st_mode))
    {
        response_code = 404;
        return;
    }

    response_code = 200;

    Content_Length = st.st_size;
    type = T_FILE;
}

void HttpResponse::SetContentLength()
{
    switch (type)
    {
    case T_FILE:
    {
        break;
    }
    case T_CONTENT:
    {
        Content_Length = content.length();
        break;
    }
    }
}

void HttpResponse::SetType()
{
    if (response_code < 100 || response_code >= 300)
    {
        if (type < 0)
        {
            type = T_CONTENT;
            content = to_string(response_code);
        }
    }
}

void HttpResponse::Clear()
{
    response_code = 200;
    Date.clear();
    Content_Type.clear();
    Content_Length = 0;
    Connection = "keep-alive";

    type = -1;
    file_path.clear();
    content.clear();
}