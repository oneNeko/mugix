#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "http_server.h"

#include "../log/log.h"
#include "../routes/routes.h"
#include "mgx_epoll.h"
#include "../config/config.h"
#include "../http/http_conn.h"
#include "../http/http_response.h"

HttpServer::HttpServer() : server_port(50001), is_run(false)
{
    users = new HttpConn[k_MAX_FD];
}

HttpServer::~HttpServer()
{
}

void HttpServer::Init()
{
    auto config = Config::get_instance();

    server_port = config->PORT;
}

void AddEvent(int epollfd, int fd, int state)
{
    struct epoll_event event;
    event.events = state;
    event.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}

void DeleteEvent(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void modify_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

// 处理新连接
bool HttpServer::dealclientdata(int listen_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);

    int connfd = accept(listen_fd, (struct sockaddr *)&client_address, &client_addrlength);

    if (connfd < 0)
    {
        return false;
    }
    Log("new client!  " + string(inet_ntoa(client_address.sin_addr)) + ":" + to_string(client_address.sin_port));
    AddEvent(m_epoll_fd, connfd, EPOLLIN);
    return true;
}

// 从socket缓冲区读取数据
void HttpServer::dealwithread(int sockfd)
{
    char buf[2048];
    int n = recv(sockfd, buf, sizeof(buf), 0);
    
    if (n < 0)
    {
        Log("recv error");
        DeleteEvent(m_epoll_fd, sockfd, EPOLLIN);
        close(sockfd);
    }
    else if (n == 0 && errno == EINTR)
    {
        Log("client close,errno=EINTR");
        DeleteEvent(m_epoll_fd, sockfd, EPOLLIN);
        close(sockfd);
    }
    else if (n == 0)
    {
        Log("client close");
        DeleteEvent(m_epoll_fd, sockfd, EPOLLIN);
        close(sockfd);
    }
    else
    {
        Log(buf);
        if (n != sizeof(buf))
        {
            users[sockfd].request_text = buf;
            users[sockfd].Process();
            modify_event(m_epoll_fd, sockfd, EPOLLOUT);
        }
        else
        {
            modify_event(m_epoll_fd, sockfd, EPOLLIN);
        }
    }
}

// 向socket缓冲区写入数据
void HttpServer::dealwithwrite(int sockfd)
{
    auto response = users[sockfd].response;
    string str_header = response.GetHeader();

    int n_write = -1;

    if (response.type == T_CONTENT)
    {
        string buf = str_header + response.content;
        n_write = write(sockfd, buf.c_str(), buf.size());
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
        n_write = write(sockfd, buf.c_str(), buf.size());
    }

    if (n_write < 0)
    {
        Log("client close,errno=EINTR");
        DeleteEvent(m_epoll_fd, sockfd, EPOLLIN);
        close(sockfd);
    }
    else
    {
        Log("response: OK");
        Log(str_header);
        modify_event(m_epoll_fd, sockfd, EPOLLIN);
    }
    users[sockfd].CloseConn();
}

// 启动监听
int HttpServer::EventListen()
{
    // 启动监听套接字
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(m_listen_fd >= 0);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(m_listen_fd, (sockaddr *)&addr, sizeof(addr));
    assert(ret >= 0);

    ret = listen(m_listen_fd, 1000);
    assert(ret >= 0);
    Log("listening at " + addr.sin_addr.s_addr + to_string(server_port));

    // epoll
    m_epoll_fd = epoll_create(5);
    assert(m_epoll_fd >= 0);

    AddEvent(m_epoll_fd, m_listen_fd, EPOLLIN);
    HttpConn::m_epollfd = m_epoll_fd;

    return 0;
}

// 主循环
void HttpServer::EventLoop()
{
    while (true)
    {
        int number = epoll_wait(m_epoll_fd, events, k_MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            Log("epoll failure");
            break;
        }

        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;

            //处理新到的客户连接
            if (sockfd == m_listen_fd)
            {
                dealclientdata(sockfd);
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                Log("client close");
                DeleteEvent(m_epoll_fd, sockfd, EPOLLIN);
                close(sockfd);
            }
            //处理客户连接上接收到的数据
            else if (events[i].events & EPOLLIN)
            {
                dealwithread(sockfd);
            }
            else if (events[i].events & EPOLLOUT)
            {
                dealwithwrite(sockfd);
            }
        }
    }
}
