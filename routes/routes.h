#ifndef ROUTES_H
#define ROUTES_H
#endif

#include <set>
#include <string>
#include "../config/define.h"
#include "../http/http_conn.h"

class ROUTES
{
private:
private:
    enum TEXT_TYPE
    {
        TYPE_TEXT = 0,
        TYPE_FILE
    };

    int parse_request_header(std::string request_header, HTTP_REQUEST_HEADER &header);

    std::string make_response(HTTP_RESPONSE response);
    std::string make_response(std::string message, int status_code = 200);
    std::string make_response(std::string message, std::string location, int status_code = 302);
    std::string make_response(std::string str, HTTP_RESPONSE_HEADER header, TEXT_TYPE type = TYPE_TEXT);

    std::string get_file_mime(std::string file_type);
    std::string get_str_status_code(int status);

    std::string login(std::string post_data);

public:
    std::string process_requests(std::string request_text);
    void ProcessRequest(HttpConn *conn);

private:
    //路由处理函数
    void ProcessFile(HttpConn *conn);
    void index(HttpConn *conn);
    void upload(HttpConn *conn);
};