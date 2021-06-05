#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include "http_server.h"

#include "../log/log.h"
#include "../routes/routes.h"
#include "../config/config.h"
#include "../http/http_conn.h"
#include "../http/http_response.h"
#include "../utils/utils.h"

HttpServer::HttpServer() : server_port_(50001)
{
    users_ = new HttpConn[k_MAX_FD];
}

HttpServer::~HttpServer()
{
}

// 服务器初始化
void HttpServer::Init()
{
    // 获取配置
    auto config = Config::GetInstance();
    server_port_ = config->PORT;
    switch (config->epoll_trige_mode_)
    {
    case 0:
    {
        epoll_trig_mode_listen_ = LT;
        epoll_trig_mode_conn_ = LT;
        break;
    }
    case 1:
    {
        epoll_trig_mode_listen_ = LT;
        epoll_trig_mode_conn_ = ET;
        break;
    }
    case 2:
    {
        epoll_trig_mode_listen_ = ET;
        epoll_trig_mode_conn_ = LT;
        break;
    }
    case 3:
    {
        epoll_trig_mode_listen_ = ET;
        epoll_trig_mode_conn_ = ET;
        break;
    }
    }

    // 创建线程池
    InitThreadPool();
}

// 处理新连接
bool HttpServer::ProcessNewClient(int listen_fd)
{
    if (epoll_trig_mode_listen_ == LT)
    {
        struct sockaddr_in client_address;
        socklen_t client_addrlength = sizeof(client_address);

        int connfd = accept(listen_fd, (struct sockaddr *)&client_address, &client_addrlength);

        if (connfd < 0)
        {
            return false;
        }
        Log("new client!  " + string(inet_ntoa(client_address.sin_addr)) + ":" + to_string(ntohs(client_address.sin_port)));
        Utils::AddEvent(epollfd_, connfd, EPOLLIN | EPOLLONESHOT);
        //Utils::ModifyEvent(epollfd_, listen_fd, EPOLLIN | EPOLLONESHOT);
        Utils::SetNonblock(connfd);
        users_[connfd].client_sockfd_ = connfd;
        users_[connfd].epoll_trig_mode_ = LT;
    }
    else
    {
        while (true)
        {
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);

            int connfd = accept(listen_fd, (struct sockaddr *)&client_address, &client_addrlength);

            if (connfd < 0)
            {
                Utils::ModifyEvent(epollfd_, listen_fd, EPOLLIN | EPOLLET | EPOLLONESHOT);
                return false;
            }
            Log("new client!  " + string(inet_ntoa(client_address.sin_addr)) + ":" + to_string(ntohs(client_address.sin_port)));
            Utils::AddEvent(epollfd_, connfd, EPOLLIN | EPOLLONESHOT | EPOLLET);
            Utils::SetNonblock(connfd);
            users_[connfd].client_sockfd_ = connfd;
            users_[connfd].epoll_trig_mode_ = EPOLLET;
        }
    }

    return true;
}

// 从socket缓冲区读取数据
void HttpServer::ProcessRead(int sockfd)
{
    char buf[2048];
    int n = recv(sockfd, buf, sizeof(buf), 0);

    if (n < 0)
    {
        Log("recv error");
        users_[sockfd].ResetConn();
    }
    else if (n == 0)
    {
        Log("client close");
        users_[sockfd].ResetConn();
    }
    else
    {
        Log(buf);

        users_[sockfd].request_text_ = buf;
        users_[sockfd].client_sockfd_ = sockfd;
        pool_->Append(users_ + sockfd);
    }
}

// 向socket缓冲区写入数据
void HttpServer::ProcessWrite(int sockfd)
{
    auto response = users_[sockfd].response_;
    string str_header = response.GetHeader();

    int n_write = -1;

    if (response.type_ == T_CONTENT)
    {
        string buf = str_header + response.content_;
        n_write = write(sockfd, buf.c_str(), buf.size());
    }
    else if (response.type_ == T_FILE)
    {
        string buf = str_header;
        n_write = write(sockfd, buf.c_str(), buf.size());
        assert(n_write > 0);

        // 发送文件
        int file_fd = open(response.file_path_.c_str(), O_RDONLY);

        n_write = sendfile(sockfd, file_fd, NULL, response.content_length_);

        assert(n_write > 0);
    }

    if (n_write < 0)
    {
        Log("client close,errno=EINTR");
        Utils::DeleteEvent(epollfd_, sockfd, EPOLLIN);
        close(sockfd);
    }
    else
    {
        Log("response: OK");
        Log(str_header);
        Utils::ModifyEvent(epollfd_, sockfd, EPOLLIN);
    }
    users_[sockfd].ResetConn();
}

// 创建线程池
void HttpServer::InitThreadPool()
{
    pool_ = new ThreadPool<HttpConn>(thread_num_, max_requests_);
}

// 启动监听
int HttpServer::EventListen()
{
    // 启动监听套接字
    server_listen_socketfd_ = socket(AF_INET, SOCK_STREAM, 0);
    assert(server_listen_socketfd_ >= 0);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port_);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int flag = 1;
    setsockopt(server_listen_socketfd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    int ret = bind(server_listen_socketfd_, (sockaddr *)&addr, sizeof(addr));
    assert(ret >= 0);

    ret = listen(server_listen_socketfd_, 1000);
    assert(ret >= 0);
    Log("listening at " + addr.sin_addr.s_addr + to_string(server_port_));

    Utils::SetNonblock(server_listen_socketfd_);

    // epoll
    epollfd_ = epoll_create(5);
    assert(epollfd_ >= 0);

    if (epoll_trig_mode_listen_ == ET)
    {
        Utils::AddEvent(epollfd_, server_listen_socketfd_, EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | epoll_trig_mode_listen_);
    }
    else
    {
        Utils::AddEvent(epollfd_, server_listen_socketfd_, EPOLLIN | EPOLLRDHUP);
    }

    HttpConn::epollfd_ = epollfd_;

    return 0;
}

// 事件主循环
void HttpServer::EventLoop()
{
    while (true)
    {
        int number = epoll_wait(epollfd_, events_, k_MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            Log("epoll failure");
            break;
        }

        for (int i = 0; i < number; i++)
        {
            int sockfd = events_[i].data.fd;

            //处理新到的客户连接
            if (sockfd == server_listen_socketfd_)
            {
                ProcessNewClient(sockfd);
            }
            else if (events_[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                Log("epoll client close");
                Utils::DeleteEvent(epollfd_, sockfd, EPOLLIN);
                close(sockfd);
            }
            //处理客户连接上接收到的数据
            else if (events_[i].events & EPOLLIN)
            {
                //Log("epoll in");
                //ProcessRead(sockfd);
                users_[sockfd].rw_state = 1;
                pool_->Append(users_ + sockfd);
            }
            else if (events_[i].events & EPOLLOUT)
            {
                //Log("epoll out");
                //ProcessWrite(sockfd);
                users_[sockfd].rw_state = 2;
                pool_->Append(users_ + sockfd);
            }
        }
    }
}
