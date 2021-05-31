#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>
#include "http_define.h"
#include <vector>

using namespace std;

class HttpRequest
{
public:
    enum
    {
        LINE_REQUEST,
        LINE_HEDAER_PARAMS,
        LINE_END,
        LINE_PENDING,
        LINE_ERROR
    };

    int method_ = -1;
    string abs_path_;
    map<string, string> uri_params_;
    string http_protocol_;
    map<string, string> header_params_;

private:
    int ParseRequestLine(string src);
    int ParseUriParams(string src);
    int ParseHeaderParams(string src);

public:
    HTTP_CODE ParseHeader(string str_request_header);
    void Clear();
};

#endif