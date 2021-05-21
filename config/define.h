#ifndef DEFINE_H
#define DEFINE_H
#endif

#include <string>
#include <unordered_map>
#include <map>

#define SS std::string, std::string
#define IS int, std::string

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
    const std::string http_protocol = "HTTP/1.1";
    int response_code = 200;

    std::unordered_map<SS> header_params = {
        std::pair<SS>("Server", "neko_server/0.1"),
        std::pair<SS>("Content-Type", "text/html;charset=utf-8"),
        std::pair<SS>("Connection", "keep-alive")};

    HTTP_RESPONSE_HEADER(){};
    HTTP_RESPONSE_HEADER(int code) : response_code(code){};
    HTTP_RESPONSE_HEADER(int code, std::string key, std::string value) : response_code(code)
    {
        header_params[key] = value;
    };
};

struct HTTP_RESPONSE
{
    HTTP_RESPONSE_HEADER header;
    std::string body;
    HTTP_RESPONSE(){};
    HTTP_RESPONSE(std::string str_body, HTTP_RESPONSE_HEADER m_header = HTTP_RESPONSE_HEADER()) : body(str_body), header(m_header){};
};


// HTTP状态码
static std::string http_status_lines[] = {

#define HTTP_OK 200

    std::string("200 OK"),
    std::string("201 Created"),
    std::string("202 Accepted"),
    std::string(""), /* "203 Non-Authoritative Information" */
    std::string("204 No Content"),
    std::string(""), /* "205 Reset Content" */
    std::string("206 Partial Content"),

/* std::string(""), */ /* "207 Multi-Status" */

#define HTTP_LAST_2XX 207
#define HTTP_OFF_3XX (HTTP_LAST_2XX - 200)

    /* std::string(""), */ /* "300 Multiple Choices" */

    std::string("301 Moved Permanently"),
    std::string("302 Moved Temporarily"),
    std::string("303 See Other"),
    std::string("304 Not Modified"),
    std::string(""), /* "305 Use Proxy" */
    std::string(""), /* "306 unused" */
    std::string("307 Temporary Redirect"),
    std::string("308 Permanent Redirect"),

#define HTTP_LAST_3XX 309
#define HTTP_OFF_4XX (HTTP_LAST_3XX - 301 + HTTP_OFF_3XX)

    std::string("400 Bad Request"),
    std::string("401 Unauthorized"),
    std::string("402 Payment Required"),
    std::string("403 Forbidden"),
    std::string("404 Not Found"),
    std::string("405 Not Allowed"),
    std::string("406 Not Acceptable"),
    std::string(""), /* "407 Proxy Authentication Required" */
    std::string("408 Request Time-out"),
    std::string("409 Conflict"),
    std::string("410 Gone"),
    std::string("411 Length Required"),
    std::string("412 Precondition Failed"),
    std::string("413 Request Entity Too Large"),
    std::string("414 Request-URI Too Large"),
    std::string("415 Unsupported Media Type"),
    std::string("416 Requested Range Not Satisfiable"),
    std::string(""), /* "417 Expectation Failed" */
    std::string(""), /* "418 unused" */
    std::string(""), /* "419 unused" */
    std::string(""), /* "420 unused" */
    std::string("421 Misdirected Request"),
    std::string(""), /* "422 Unprocessable Entity" */
    std::string(""), /* "423 Locked" */
    std::string(""), /* "424 Failed Dependency" */
    std::string(""), /* "425 unused" */
    std::string(""), /* "426 Upgrade Required" */
    std::string(""), /* "427 unused" */
    std::string(""), /* "428 Precondition Required" */
    std::string("429 Too Many Requests"),

#define HTTP_LAST_4XX 430
#define HTTP_OFF_5XX (HTTP_LAST_4XX - 400 + HTTP_OFF_4XX)

    std::string("500 Internal Server Error"),
    std::string("501 Not Implemented"),
    std::string("502 Bad Gateway"),
    std::string("503 Service Temporarily Unavailable"),
    std::string("504 Gateway Time-out"),
    std::string("505 HTTP Version Not Supported"),
    std::string(""), /* "506 Variant Also Negotiates" */
    std::string("507 Insufficient Storage"),

/* std::string(""), */ /* "508 unused" */
/* std::string(""), */ /* "509 unused" */
/* std::string(""), */ /* "510 Not Extended" */

#define HTTP_LAST_5XX 508

};