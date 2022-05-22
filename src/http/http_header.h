#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

namespace mugix
{
#pragma region http定义
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

    // http响应代码
    enum HttpResponseCode
    {
        CODE_100 = 0,
        CODE_200,
        CODE_206,
        CODE_301,
        CODE_302,
        CODE_400,
        CODE_403,
        CODE_404,
        CODE_500,
        CODE_503
    };
    const unordered_map<int, string> http_response_code_ = {
        {CODE_100, "100 Continue"},
        {CODE_200, "200 OK"},
        {CODE_206, "206 Partial Content"},
        {CODE_301, "301 Movied Permanently"},
        {CODE_302, "302 Found"},
        {CODE_400, "400 Bad Request"},
        {CODE_403, "403 Forbidden"},
        {CODE_404, "404 Not Found"},
        {CODE_500, "500 Internal Server Error"},
        {CODE_503, "503 Service Unavailable"},
    };
#pragma endregion

    // http请求头结构
    class HttpHeader
    {
    private:
        //哈希表映射
        template <typename T1, typename T2>
        int ParseHeaderMap(T1 &key, T2 &hash_table)
        {
            if (hash_table.count(key) != 0)
            {
                return hash_table.at(key);
            }
            return 0;
        }
        int ParseHeader(const string &header);

    public:
        HttpHeader();
        HttpHeader(const string &header);

    public:
        int method_ = HTTP_METHOD_NOT_SUPPORT;   //http请求方法
        string path_ = "";                       //http请求路径
        int version_ = HTTP_VERSION_NOT_SUPPORT; //http版本
        string host_ = "";                       //http请求主机
        string connection_ = "";                 //http连接方式
        string cookie_ = "";                     //http请求cookie
        //unordered_map<string, string> params_;//http其他参数

        //http响应
        //必选
        int reqponse_code_ = CODE_200; //http响应代码
        string server_ = "";           //http响应服务器
        string content_length_ = "";   //http响应长度
        string content_type_ = "";     //http响应类型
        //可选
        string location_ = "";         //http响应重定向
        string content_language_ = ""; //http响应语言
    };

} //namespace mugix
#endif