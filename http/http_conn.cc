#include "http_conn.h"
#include "../utils/utils.h"
#include "../routes/routes.h"
#include <iostream>
#include <assert.h>
#include "../log/log.h"
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <ostream>
#include <istream>
#include <sstream>
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

void delete_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

bool HttpConn::process_write(HTTP_CODE ret)
{
    ROUTES route;
    route.ProcessRequest(this);
    response.Process();

    string str_header = response.GetHeader();

    int n_write = -1;

    if (response.type == T_CONTENT)
    {
        string buf = str_header + response.content;
        n_write = write(m_sockfd, buf.c_str(), buf.size());
        assert(n_write > 0);
    }
    else if (response.type == T_FILE)
    {
        std::ifstream infile;
        std::stringstream buffer;

        infile.open(response.file_path);

        buffer << infile.rdbuf();
        std::string contents(buffer.str());

        infile.close();

        string buf = str_header + contents;
        n_write = write(m_sockfd, buf.c_str(), buf.size());
        assert(n_write > 0);
    }

    if (n_write < 0)
    {
        Log("client close,errno=EINTR");
        delete_event(m_epollfd, m_sockfd, EPOLLIN);
        close(m_sockfd);
    }
    else
    {
        Log("response: OK");
        Log(str_header);
        Utils::ModifyFd(m_epollfd, m_sockfd, EPOLLIN);
    }
    CloseConn();

    close(m_sockfd);

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
    //Utils::ModifyFd(m_epollfd, m_sockfd, EPOLLOUT);
}