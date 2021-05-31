#include "http_request.h"
#include <iostream>

#include "../utils/utils.h"

// 解析请求行
int HttpRequest::ParseRequestLine(string src)
{
    auto items = Utils::SplitString(src, " ");
    if (items.size() != 3)
    {
        return BAD_REQUEST;
    }

    // 解析请求方法
    for (int i = 0; i < 2; i++)
    {
        if (method_str[i] == items[0])
        {
            method_ = i;
        }
    }
    if (method_ == -1)
    {
        return -1;
    }

    int res = ParseUriParams(items[1]);
    if (res != 0)
    {
        return 10;
    }
    http_protocol_ = items[2];
    return 0;
}

// 解析uri参数列表
int HttpRequest::ParseUriParams(string src)
{
    int pos = 0;

    // 解析路径
    pos = int(src.find("?"));
    if (pos > src.size())
    {
        abs_path_ = src;
        return 0;
    }

    abs_path_ = src.substr(0, pos);
    src = src.substr(pos + 1, src.length() - pos);

    // 解析参数
    auto params = Utils::SplitString(src, "&");
    for (auto it : params)
    {
        auto its = Utils::SplitString(it, "=");
        if (its.size() != 2)
        {
            return -1;
        }
        uri_params_[its[0]] = its[1];
    }
    return 0;
}

// 解析请求头参数
int HttpRequest::ParseHeaderParams(string src)
{
    auto params = Utils::SplitString(src, ": ");
    if (params.size() != 2)
    {
        return BAD_REQUEST;
    }
    header_params_[params[0]] = params[1];
    return 0;
}

// 解析请求头
HTTP_CODE HttpRequest::ParseHeader(string str_request_header)
{
    int left = 0, right = 0;
    bool check_request_line = true;
    int state = LINE_PENDING;
    int length = str_request_header.length();
    string line_to_process = "";

    while (true)
    {
        switch (state)
        {
        case LINE_REQUEST:
        {
            if (0 != ParseRequestLine(line_to_process))
            {
                state = LINE_ERROR;
            }
            check_request_line = false;
            state = LINE_PENDING;
            break;
        }
        case LINE_PENDING:
        {
            right = int(str_request_header.find("\r\n", left));
            if (right == -1)
            {
                if (left >= length)
                {
                    state = LINE_END;
                    break;
                }
                line_to_process = str_request_header.substr(left);
                right = INT16_MAX - 10;
            }
            else
            {
                line_to_process = str_request_header.substr(left, right - left);
            }

            left = right + 2;

            if (check_request_line) //解析请求行
            {
                state = LINE_REQUEST;
            }
            else
            {
                state = LINE_HEDAER_PARAMS;
            }
            break;
        }
        case LINE_HEDAER_PARAMS:
        {
            if (0 != ParseHeaderParams(line_to_process))
            {
                state = LINE_ERROR;
            }
            state = LINE_PENDING;
            break;
        }
        case LINE_ERROR:
        {
            return BAD_REQUEST;
            break;
        }
        case LINE_END:
        {
            return GET_REQUEST;
            break;
        }
        }
    }
    return GET_REQUEST;
}

void HttpRequest::Clear()
{
    method_ = -1;
    abs_path_.clear();
    uri_params_.clear();
    http_protocol_.clear();

    header_params_.clear();
}