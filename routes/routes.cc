#include "../utils/utils.h"
#include "../log/log.h"
#include "routes.h"
#include "../config/config.h"

std::string ROUTES::get_file_mime(std::string file_type)
{
    auto instance = Config::GetInstance();
    if (instance->content_type_.find(file_type) != instance->content_type_.end())
    {
        return instance->content_type_[file_type];
    }
    return "null";
}

void ROUTES::ProcessRequest(HttpConn *conn)
{
    if (conn->request_.abs_path_ == "/")
    {
        index(conn);
    }
    else if (conn->request_.abs_path_ == "/upload")
    {
        upload(conn);
    }
    else
    {
        ProcessFile(conn);
    }
}

#pragma region 路由处理函数

void ROUTES::ProcessFile(HttpConn *conn)
{
    conn->response_.file_path_ = conn->request_.abs_path_;
    string file_type = conn->response_.file_path_.substr(conn->response_.file_path_.find_last_of(".") + 1);
    conn->response_.content_type_ = get_file_mime(file_type);
}

void ROUTES::index(HttpConn *conn)
{
    conn->request_.abs_path_ = "/index.html";
    ProcessFile(conn);
}

void ROUTES::upload(HttpConn *conn)
{
}

#pragma endregion
