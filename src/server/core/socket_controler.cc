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

#include "socket_controler.h"

namespace mugix::server
{
    // 从socket中读取数据后交由http层处理
    int ReadFromSocket(int fd)
    {
        char buffer[2048];
        int n = recv(fd, buffer, sizeof(buffer), 0);
        if (n < 0)
        {
            close(fd);
        }
        else if (n == 0)
        {
            close(fd);
        }
        else
        {
            // 待完成
        }
        return n;
    }

    // 向socket写入
    int WriteToSocket(int fd)
    {
        // 待完成
        return 0;
    }

#pragma region 核心

    // 初始化mugix核心
    bool SockerControler::Init()
    {
        // 读取配置

        // 初始化线程池
        pool_.init();
        return true;
    }

    // 启动监听
    bool SockerControler::EventListen()
    {
        // 启动监听套接字
        server_listen_socketfd_ = socket(AF_INET, SOCK_STREAM, 0);
        assert(server_listen_socketfd_ >= 0);

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server_port_);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        assert(bind(server_listen_socketfd_, (sockaddr *)&addr, sizeof(addr)) >= 0);
        assert(listen(server_listen_socketfd_, 1000) >= 0);

        bool flag = true;
        setsockopt(server_listen_socketfd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(bool)); //允许监听套接字重复使用，跳过time_wait

        //设置监听套接字非阻塞
        SetSocketNonBlock(server_listen_socketfd_);

        // 启动epoll
        epollfd_ = epoll_create(5);
        assert(epollfd_ >= 0);

        // 设置为边缘触发时，设置为oneshot模式，避免收不到持续信息
        if (mode_epoll_trig_listen_ == EPOLL_ET)
        {
            AddEpollEvent(server_listen_socketfd_, EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | mode_epoll_trig_listen_);
        }
        else
        {
            AddEpollEvent(server_listen_socketfd_, EPOLLIN | EPOLLRDHUP);
        }
        return true;
    }

    // 事件主循环
    bool SockerControler::EventLoop()
    {
        while (true)
        {
            int number = epoll_wait(epollfd_, events_, k_MAX_EVENT_NUMBER, -1);
            if (number < 0 && errno != EINTR)
            {
                //logger->fatal("epoll failure");
                break;
            }

            for (int i = 0; i < number; i++)
            {
                int sockfd = events_[i].data.fd;

                //有新到的客户连接事件
                if (sockfd == server_listen_socketfd_)
                {
                    ProcessNewClient();
                }
                //收到epoll断开或错误事件
                else if (events_[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
                {
                    //logger->error("epoll client close");
                    DeleteEpollEvent(sockfd, EPOLLIN);
                    close(sockfd);
                }
                //收到epoll写入事件
                else if (events_[i].events & EPOLLIN)
                {
                    pool_.submit(ReadFromSocket, sockfd);
                }
                //收到epoll输出事件
                else if (events_[i].events & EPOLLOUT)
                {
                    pool_.submit(WriteToSocket, sockfd);
                }
            }
        }
        return true;
    }

    // 处理新到的连接
    bool SockerControler::ProcessNewClient()
    {
        // 水平触发方式
        if (mode_epoll_trig_listen_ == EPOLL_LT)
        {
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);

            int connfd = accept(server_listen_socketfd_, (struct sockaddr *)&client_address, &client_addrlength);

            if (connfd < 0)
            {
                return false;
            }

            AddEpollEvent(connfd, EPOLLIN | EPOLLONESHOT);
            SetSocketNonBlock(connfd);

            users_[connfd] = UserSocket(connfd, client_address, EPOLL_LT);
        }
        // 边缘触发方式
        else
        {
            while (true)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);

                int connfd = accept(server_listen_socketfd_, (struct sockaddr *)&client_address, &client_addrlength);

                if (connfd < 0)
                {
                    ModifyEpollEvent(server_listen_socketfd_, EPOLLIN | EPOLLET | EPOLLONESHOT);
                    return false;
                }

                AddEpollEvent(connfd, EPOLLIN | EPOLLONESHOT | EPOLLET);
                SetSocketNonBlock(connfd);
                users_[connfd] = UserSocket(connfd, client_address, EPOLL_LT);
            }
        }

        return true;
    }

    void SockerControler::run()
    {
        Init();
        EventListen();
        EventLoop();
    }

#pragma endregion

#pragma region 处理epoll事件
    // 添加epoll事件
    void SockerControler::AddEpollEvent(int fd, int state)
    {
        struct epoll_event event;
        event.events = state;
        event.data.fd = fd;
        epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event);
    }

    // 删除epoll事件
    void SockerControler::DeleteEpollEvent(int fd, int state)
    {
        struct epoll_event ev;
        ev.events = state;
        ev.data.fd = fd;
        epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev);
    }

    // 修改epoll事件
    void SockerControler::ModifyEpollEvent(int fd, int state)
    {
        struct epoll_event ev;
        ev.events = state;
        ev.data.fd = fd;
        epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
    }

    // 设置套接字非阻塞
    void SockerControler::SetSocketNonBlock(int fd)
    {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
}
#pragma endregion
