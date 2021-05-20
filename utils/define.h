#ifndef DEFINE_H
#define DEFINE_H
#endif

#include <string>
#include <unordered_map>

#define SS std::string, std::string

// HTTP类型
enum HTTP_TYPE
{
    GET = 1,
    POST,
    HEAD
};

// HTTP请求头结构
struct HTTP_REQUEST_HEADER
{
    int http_type = GET;
    std::string abs_path = "/";
    std::unordered_map<SS> uri_params;
    std::string http_protocol;

    std::string hosts = "0.0.0.0";
    int port = 0;

    std::unordered_map<SS> header_params;
};

struct HTTP_REQUEST_TEXT
{
    HTTP_REQUEST_HEADER header;
    std::string body;
};

// HTTP响应头结构
struct HTTP_RESPONSE_HEADER
{
    std::string http_protocol = "HTTP/1.1";
    int response_code = 200;

    std::unordered_map<SS> header_params = {
        std::pair<SS>("Server", "neko_server/0.1"),
        std::pair<SS>("Date", ""),
        std::pair<SS>("Content-Type", ""),
        std::pair<SS>("Connection", "keep-alive")};
};

struct HTTP_RESPONSE_TEXT
{
    HTTP_RESPONSE_HEADER header;
    std::string body;
};