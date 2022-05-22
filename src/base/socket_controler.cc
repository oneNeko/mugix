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
#include <functional>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

#include "config.h"
#include "socket_controler.h"
#include "../utils/logger.h"
#include "../utils/utils.h"

#pragma region 处理epoll事件
// 添加epoll事件
void AddEpollEvent(int epoll_fd, int fd, int state)
{
    struct epoll_event event;
    event.events = state;
    event.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
}

// 删除epoll事件
void DeleteEpollEvent(int epoll_fd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev);
}

// 修改epoll事件
void ModifyEpollEvent(int epoll_fd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

// 设置套接字非阻塞
void SetSocketNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

#pragma endregion

namespace mugix::server
{
    std::mutex in_mutex, out_mutex;
    // 从socket中读取数据后交由http层处理
    int SocketControler::ReadFromSocket(int sock_fd)
    {
        debug("开始读取scoket缓冲区...");
        char buffer[2048];
        memset(buffer, 0, 2048);
        int n = recv(sock_fd, buffer, sizeof(buffer), 0);
        debug("接收到 %d bytes", n);
        if (n < 0)
        {
            debug("n<0,关闭sockfd=%d", sock_fd);
            close(sock_fd);
            DeleteEpollEvent(epollfd_, sock_fd, EPOLLIN);
        }
        else if (n == 0)
        {
            debug("n=0,关闭sockfd=%d", sock_fd);
            close(sock_fd);
            DeleteEpollEvent(epollfd_, sock_fd, EPOLLIN);
        }
        else
        {
            info("接收到内容：%s", buffer);

            users_[sock_fd].http_controler_.url_ = HttpHeader(buffer).path_;
            ModifyEpollEvent(epollfd_, sock_fd, EPOLLOUT | EPOLLONESHOT);
        }
        debug("读取scoket缓冲区结束");
        return n;
    }

    // 向socket写入
    int SocketControler::WriteToSocket(int sock_fd)
    {
        debug("开始发送数据到scoket缓冲区...");
        const char *header = "HTTP/1.1 %d OK\r\nServer: nginx/1.18.0 (Ubuntu)\r\nContent-Length:%d\r\n\r\n";
        std::string body = "404";

        int n_send = 0;
        char response[1024];

        if (users_[sock_fd].http_controler_.url_ == "/")
        {
            users_[sock_fd].http_controler_.url_ = "index.html";
        }

        std::string file_path = Config::GetConfig().root_path_ + UrlDecode(users_[sock_fd].http_controler_.url_.substr(0, users_[sock_fd].http_controler_.url_.find('?')));

        int file_fd = open(file_path.c_str(), O_RDONLY);
        if (file_fd <= 0)
        {
            error("打开文件失败，url=%s", users_[sock_fd].http_controler_.url_.c_str());
            sprintf(response, header, 404, body.length());
            strcat(response, body.c_str());
            n_send = send(sock_fd, response, strlen(response), 0);
        }
        else
        {
            struct stat stat_buf;
            fstat(file_fd, &stat_buf);
            sprintf(response, header, 200, stat_buf.st_size);
            stat_buf.st_mtim;

            // 校验时间
            // char m_time[1024];
            // struct tm tm1;
            // tm1 = *localtime(&stat_buf.st_mtim.tv_sec);
            // sprintf(m_time, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d", tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
            //         tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
            // info(m_time);
            
            n_send = send(sock_fd, response, strlen(response), 0);
            n_send += sendfile(sock_fd, file_fd, 0, stat_buf.st_size);
        }

        debug("发送字节数n=%d", n_send);
        if (n_send <= 0)
        {
            debug("发送了 %d bytes，关闭socket", n_send);
            close(sock_fd);
            DeleteEpollEvent(epollfd_, sock_fd, EPOLLIN);
        }
        else
        {
            debug("发送成功");
        }

        debug("发送到scoket缓冲区结束");
        return 0;
    }

#pragma region 核心

    // 初始化mugix核心
    bool SocketControler::Init()
    {
        // 读取配置
        mode_epoll_trig_listen_ = EPOLL_ET;

        // 初始化线程池
        pool_.init();
        return true;
    }

    // 启动监听
    bool SocketControler::EventListen()
    {
        // 启动监听套接字
        server_listen_socketfd_ = socket(AF_INET, SOCK_STREAM, 0);
        assert(server_listen_socketfd_ >= 0);

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server_port_);
        // addr.sin_addr.s_addr = htonl(INADDR_ANY);
        inet_pton(AF_INET, server_listen_ip_, &addr.sin_addr);

        bool flag = true;
        setsockopt(server_listen_socketfd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(bool)); //允许监听套接字重复使用，跳过time_wait(必须在bind之前使用)

        assert(bind(server_listen_socketfd_, (sockaddr *)&addr, sizeof(addr)) >= 0);
        assert(listen(server_listen_socketfd_, 1000) >= 0);

        // 输出客户端ip 端口
        info("开始监听，ip=%s,port=%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

        //设置监听套接字非阻塞
        SetSocketNonBlock(server_listen_socketfd_);

        // 启动epoll
        epollfd_ = epoll_create(5);
        assert(epollfd_ >= 0);

        // 设置为边缘触发时，设置为oneshot模式，避免收不到持续信息
        if (mode_epoll_trig_listen_ == EPOLL_ET)
        {
            AddEpollEvent(epollfd_, server_listen_socketfd_, EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | mode_epoll_trig_listen_);
        }
        else
        {
            AddEpollEvent(epollfd_, server_listen_socketfd_, EPOLLIN | EPOLLRDHUP);
        }
        return true;
    }

    // 事件主循环
    bool SocketControler::EventLoop()
    {
        while (true)
        {
            int number = epoll_wait(epollfd_, events_, k_MAX_EVENT_NUMBER, -1);
            if (number < 0 && errno != EINTR)
            {
                // logger->fatal("epoll failure");
                break;
            }

            for (int i = 0; i < number; i++)
            {
                int sockfd = events_[i].data.fd;

                //有新到的客户连接事件
                if (sockfd == server_listen_socketfd_)
                {
                    debug("epoll 接收到新连接");
                    ProcessNewClient();
                }
                //收到epoll断开或错误事件
                else if (events_[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
                {
                    // logger->error("epoll client close");
                    debug("epoll 断开或错误，关闭socket,sockfd=%d", sockfd);
                    DeleteEpollEvent(epollfd_, sockfd, EPOLLIN);
                    close(sockfd);
                }
                //收到epoll写入事件
                else if (events_[i].events & EPOLLIN)
                {
                    pool_.submit(std::bind(&SocketControler::ReadFromSocket, this, sockfd)); //向线程池添加读任务
                }
                //收到epoll输出事件
                else if (events_[i].events & EPOLLOUT)
                {
                    pool_.submit(std::bind(&SocketControler::WriteToSocket, this, sockfd)); //向线程池添加写任务
                    ModifyEpollEvent(epollfd_, sockfd, EPOLLIN | EPOLLONESHOT);
                }
            }
        }
        return true;
    }

    // 处理新到的连接
    bool SocketControler::ProcessNewClient()
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

            AddEpollEvent(epollfd_, connfd, EPOLLIN | EPOLLONESHOT);
            SetSocketNonBlock(connfd);

            users_[connfd] = UserSocket(connfd, client_address, EPOLL_LT);

            // 输出客户端ip 端口
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            getpeername(connfd, (struct sockaddr *)&addr, &len);
            info("新客户端，ip=%s,port=%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
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
                    ModifyEpollEvent(epollfd_, server_listen_socketfd_, EPOLLIN | EPOLLET | EPOLLONESHOT);
                    return false;
                }

                AddEpollEvent(epollfd_, connfd, EPOLLIN | EPOLLONESHOT | EPOLLET);
                SetSocketNonBlock(connfd);
                users_[connfd] = UserSocket(connfd, client_address, EPOLL_LT);

                // 输出客户端ip 端口
                struct sockaddr_in addr;
                socklen_t len = sizeof(addr);
                getpeername(connfd, (struct sockaddr *)&addr, &len);
                info("新客户端，ip=%s,port=%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            }
        }

        return true;
    }

    void SocketControler::run()
    {
        Init();
        EventListen();
        EventLoop();
    }

#pragma endregion
}
