#ifndef ROUTES_H
#define ROUTES_H

#include <string>
#include "../http/http_conn.h"

class ROUTES
{
private:
    enum TEXT_TYPE
    {
        TYPE_TEXT = 0,
        TYPE_FILE
    };
    std::string get_file_mime(std::string file_type);
    std::string get_str_status_code(int status);

public:
    std::string process_requests(std::string request_text);
    void ProcessRequest(HttpConn *conn);

private:
    //路由处理函数
    void ProcessFile(HttpConn *conn);

    // 路由(函数名与请求路径一致)
    void index(HttpConn *conn);
    void upload(HttpConn *conn);
};

#endif