#include "http_request.h"
#include <iostream>

vector<string> HttpRequest::SplitString(string src, string pattern)
{
    int left = 0, right = 0;
    int pattern_size = pattern.length();
    int src_length = src.length();
    std::vector<std::string> result;
    while (true)
    {
        right = int(src.find(pattern, left));
        if (right == -1)
        {
            if (left == src_length)
            {
                break;
            }
            result.push_back(src.substr(left));
            break;
        }
        result.push_back(src.substr(left, right - left));
        left = right + pattern_size;
    }
    return result;
}

int HttpRequest::ParseRequestLine(string src)
{
    auto items = SplitString(src, " ");
    if (items.size() != 3)
    {
        return BAD_REQUEST;
    }

    // 解析请求方法
    for (int i = 0; i < 2; i++)
    {
        if (method_str[i] == items[0])
        {
            m_method = i;
        }
    }
    if (m_method == -1)
    {
        return -1;
    }

    int res = ParseUriParams(items[1]);
    if (res != 0)
    {
        return 10;
    }
    http_protocol = items[2];
    return 0;
}
int HttpRequest::ParseUriParams(string src)
{
    int pos = 0;

    // 解析路径
    pos = int(src.find("?"));
    if (pos > src.size())
    {
        abs_path = src;
        return 0;
    }

    abs_path = src.substr(0, pos);
    src = src.substr(pos + 1, src.length() - pos);

    // 解析参数
    auto params = SplitString(src, "&");
    for (auto it : params)
    {
        auto its = SplitString(it, "=");
        if (its.size() != 2)
        {
            return -1;
        }
        uri_params[its[0]] = its[1];
    }
    return 0;
}
int HttpRequest::ParseHeaderParams(string src)
{
    auto params = SplitString(src, ": ");
    if (params.size() != 2)
    {
        return BAD_REQUEST;
    }
    header_params[params[0]] = params[1];
    return 0;
}

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
                right = INT16_MAX-10;
            }
            else
            {
                line_to_process = str_request_header.substr(left, right - left);
            }

            left = right + 2;

            if (check_request_line)
            {
                state = LINE_REQUEST;
            }
            else
            {
                state = LINE_HEDAER_PARAMS;
            }
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