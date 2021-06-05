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

    // 断点续传
    if (conn->request_.header_params_.find("Range") != conn->request_.header_params_.end())
    {
        auto res = Utils::SplitString(conn->request_.header_params_["Range"], "-");
        if (res.size() == 1)
        {
            conn->response_.big_file_offset_left_ = atoi(res[0].substr(6).c_str());
        }
        else
        {
            conn->response_.big_file_offset_left_ = atoi(res[0].substr(6).c_str());
            conn->response_.big_file_offset_right_ = atoi(res[1].c_str());
        }
    }
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
