#ifndef HTTP_DEFINE_H
#define HTTP_DEFINE_H

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

namespace mugix::http
{
#pragma region Http头定义
    //http头
    enum HeaderPos
    {
        HEADER_START = 0,    //开始
        HEADER_METHOD,       //方法
        HEADER_PATH,         //路径
        HEADER_VERSION,      //版本
        HEADER_OTHER_PARAMS, //其他属性
        HEADER_END           //结束
    };

    // http请求方法
    enum HttpMethod
    {
        HTTP_METHOD_NOT_SUPPORT = 0,
        GET,
        POST,
        HEAD,
    };
    const unordered_map<string, int> http_methods_ = {
        {"GET", GET},
        {"POST", POST},
        {"HEAD", HEAD}};

    // http版本
    enum HttpVersion
    {
        HTTP_VERSION_NOT_SUPPORT = 0,
        HTTP_10,
        HTTP_11,
    };
    const unordered_map<string, int> http_versions_ = {
        {"HTTP/1.0", HTTP_10},
        {"HTTP/1.1", HTTP_11},
    };

    // http请求头结构
    struct HttpHeader
    {
        int method_ = HTTP_METHOD_NOT_SUPPORT;
        string path_ = "/";
        int version_ = HTTP_VERSION_NOT_SUPPORT;
        string host_ = "", connection_ = "", cookie_ = "";
        unordered_map<string, string> params_;
    };
#pragma endregion
}
#endif