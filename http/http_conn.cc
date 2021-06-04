#include <assert.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "http_conn.h"
#include "../utils/utils.h"
#include "../routes/routes.h"
#include "../log/log.h"

using namespace std;

int HttpConn::user_count_ = 0;
int HttpConn::epollfd_ = -1;

void HttpConn::Init(int sock_fd, const sockaddr_in &addr)
{
    client_sockfd_ = sock_fd;
    client_address_ = addr;

    Init();
}

void HttpConn::Init()
{
}

void HttpConn::ResetConn(bool real_close)
{
    request_text_.clear();
    request_.Clear();
    response_.Clear();
    if (real_close)
    {
        Log("Reset Conn: client close");
        Utils::DeleteEvent(epollfd_, client_sockfd_, EPOLLIN);
        close(client_sockfd_);
    }
}

HTTP_CODE HttpConn::ProcessRead()
{
    int pos = request_text_.find("\r\n\r\n");

    if (pos < 0)
    {
        return BAD_REQUEST;
    }
    string header = request_text_.substr(0, pos);

    HTTP_CODE res = request_.ParseHeader(header);

    ROUTES route;
    route.ProcessRequest(this);

    return res;
}
HTTP_CODE HttpConn::ProcessWrite()
{
    response_.Process();

    str_header = response_.GetHeader();
    return GET_REQUEST;
}
bool HttpConn::ReadFromSocket()
{
    char buf[2048];
    int n = recv(client_sockfd_, buf, sizeof(buf), 0);

    if (n <= 0)
    {
        return false;
    }
    else
    {
        Log(buf);
        request_text_ = buf;
    }
    return true;
}
bool HttpConn::WriteToSocket()
{
    int n_write = -1;

    if (response_.type_ == T_CONTENT)
    {
        string buf = str_header + response_.content_;
        n_write = write(client_sockfd_, buf.c_str(), buf.size());
    }
    else if (response_.type_ == T_FILE)
    {
        std::ifstream infile;
        std::stringstream buffer;

        infile.open(response_.file_path_);

        buffer << infile.rdbuf();
        std::string contents(buffer.str());

        infile.close();

        string buf = str_header + contents;
        n_write = write(client_sockfd_, buf.c_str(), buf.size());
    }

    return n_write > 0;
}

// 处理请求
void HttpConn::Process()
{
    if (rw_state == 1)
    {
        if (!ReadFromSocket())
        {
            ResetConn();
            return;
        }
        ProcessRead();
        Utils::ModifyEvent(epollfd_, client_sockfd_, EPOLLOUT | EPOLLONESHOT | epoll_trig_mode_);
        return;
    }
    else if (rw_state == 2)
    {
        ProcessWrite();
        if (!WriteToSocket())
        {
            ResetConn();
            return;
        }

        if (request_.http_protocol_ == "HTTP/1.0")
        {
            ResetConn();
        }
        else
        {
            // http1.1协议默认保持长连接，需要将epoll事件添加回来
            Log("长连接");
            ResetConn(false);
            Utils::ModifyEvent(epollfd_, client_sockfd_, EPOLLIN | EPOLLONESHOT | epoll_trig_mode_);
        }
    }
}