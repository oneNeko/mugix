#include "http_conn.h"
#include "../utils/utils.h"
#include "../routes/routes.h"
#include <iostream>
using namespace std;

int HttpConn::m_user_count = 0;
int HttpConn::m_epollfd = -1;

void add_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void HttpConn::Init(int sock_fd, const sockaddr_in &addr)
{
    m_sockfd = sock_fd;
    m_address = addr;

    add_event(m_epollfd, sock_fd, EPOLLIN);

    Init();
}

void HttpConn::Init()
{
}

void HttpConn::CloseConn(bool real_close)
{
    request_text = "";
    request.Clear();
    response.Clear();
}

bool HttpConn::write()
{
    return true;
}

HTTP_CODE HttpConn::ProcessRead()
{
    int pos = request_text.find("\r\n\r\n");

    if (pos < 0)
    {
        return BAD_REQUEST;
    }
    string header = request_text.substr(0, pos);

    HTTP_CODE res = request.ParseHeader(header);

    return res;
}

bool HttpConn::process_write(HTTP_CODE ret)
{
    ROUTES route;
    route.ProcessRequest(this);
    response.Process();
    return true;
}

HTTP_CODE HttpConn::parse_request_line(char *text)
{
    return GET_REQUEST;
}

HTTP_CODE HttpConn::parse_headers(char *text)
{
    return GET_REQUEST;
}

HTTP_CODE HttpConn::parse_content(char *text)
{
    return GET_REQUEST;
}

HTTP_CODE HttpConn::do_request()
{
    return GET_REQUEST;
}

// 处理请求
void HttpConn::Process()
{
    HTTP_CODE read_ret = ProcessRead();
    if (read_ret == NO_REQUEST)
    {
        Utils::ModifyFd(m_epollfd, m_sockfd, EPOLLIN);
        return;
    }
    bool write_ret = process_write(read_ret);
    if (!write_ret)
    {
        CloseConn();
    }
    Utils::ModifyFd(m_epollfd, m_sockfd, EPOLLOUT);
}