#include <assert.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <sys/mman.h>
#include <sys/uio.h>
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

HttpConn::HttpConn()
{
    logger = Log::GetInstance();
}

HttpConn::~HttpConn()
{
}

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
    memset(header_buf_, 0, sizeof(header_buf_));
    request_text_.clear();
    request_.Clear();
    response_.Clear();
    if (real_close)
    {
        logger->debug("Reset Conn: client close");
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
    strcpy(header_buf_, response_.GetHeader().c_str());

    iov[0].iov_base = header_buf_;
    iov[0].iov_len = strlen(header_buf_);

    /*if (response_.type_ == T_FILE)
    {
        int fd = open(response_.file_path_.c_str(), O_RDONLY);
        file_address_ = (char *)mmap(0, response_.content_length_, PROT_READ, MAP_PRIVATE, fd, 0);

        iov[1].iov_base = file_address_;
        iov[1].iov_len = response_.content_length_;
        iv_count_ = 2;
        close(fd);
    }*/

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
        logger->debug(buf);
        request_text_ = buf;
    }
    return true;
}
bool HttpConn::WriteToSocket()
{
    int n_write = -1;

    if (response_.type_ == T_CONTENT)
    {
        string buf = header_buf_ + response_.content_;
        n_write = write(client_sockfd_, buf.c_str(), buf.size());
    }
    else if (response_.type_ == T_FILE)
    {
        string buf = header_buf_;
        int send_bytes = 0;
        n_write = write(client_sockfd_, buf.c_str(), buf.size());
        logger->debug(buf.c_str());

        int ret = 0, left = response_.content_length_;
        int fd = open(response_.file_path_.c_str(), O_RDONLY);

        off_t off_set = 0;
        while (send_bytes < response_.content_length_)
        {
            ret = sendfile(client_sockfd_, fd, &off_set, response_.content_length_);
            if (ret >= 0)
            {
                send_bytes += ret;
                off_set = send_bytes;
            }
            else if (ret < 0 && errno == EAGAIN)
            {
                continue;
            }
            else
            {
                break;
            }
        }

        close(fd);
    }
    else if (response_.type_ == T_BIG_FILE)
    {
        string buf = header_buf_;
        n_write = write(client_sockfd_, buf.c_str(), buf.size());
        logger->debug(buf.c_str());

        int ret = 0, left = response_.content_length_;
        int fd = open(response_.file_path_.c_str(), O_RDONLY);

        off_t off_set = response_.big_file_offset_left_;
        int buf_length = 1024;

        assert(fd > 0);
        while (left > 0)
        {
            ret = sendfile(client_sockfd_, fd, &off_set, response_.content_length_);
            //printf("client_sockfd_=%d,fd=%d,ret=%d,off_set=%ld\n", client_sockfd_, fd, ret, off_set);
            if (ret < 0 && errno == EAGAIN)
            {
                continue;
            }
            else if (ret == 0)
            {
                break;
            }
            else
            {
                left -= ret;
            }
        }
        close(fd);
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
        logger->info("");
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
            ResetConn(false);
            Utils::ModifyEvent(epollfd_, client_sockfd_, EPOLLIN | EPOLLONESHOT | epoll_trig_mode_);
        }
    }
}