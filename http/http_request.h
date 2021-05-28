#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>
#include "http_define.h"
#include <vector>

using namespace std;

class HttpRequest
{
private:
    enum
    {
        LINE_REQUEST,
        LINE_HEDAER_PARAMS,
        LINE_END,
        LINE_PENDING,
        LINE_ERROR
    };

    int m_method = -1;
    string abs_path;
    map<string, string> uri_params;
    string http_protocol;

    map<string, string> header_params;
    string Host;
    string Connection;
    string User_Agent;
    string Cookie;

private:
    vector<string> SplitString(string src, string pattern);

private:
    int ParseRequestLine(string src);
    int ParseUriParams(string src);
    int ParseHeaderParams(string src);

public:
    HTTP_CODE ParseHeader(string str_request_header);
};

#endif