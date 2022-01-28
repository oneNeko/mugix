#ifndef SOCKET_CONTROLER_H
#define SOCKET_CONTROLER_H

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "thread_pool.h"

namespace mugix::server
{
    const int k_MAX_FD = 65536;
    const int k_MAX_EVENT_NUMBER = 10000;
    const int k_MAX_EPOLL = 1000;

    // socket相关
    class UserSocket
    {
        int socket_fd_;                     //用户套接字
        struct sockaddr_in client_address_; //用户ip地址
        int epoll_mode_;                    //epoll触发模式
    public:
        UserSocket(){};
        UserSocket(int fd, sockaddr_in address, int epoll_mode) : socket_fd_(fd), client_address_(address){};
    };

    // mugix核心
    // 使用epoll管理socket
    // 使用线程池处理请求
    class SockerControler
    {
    private:
        // epoll 触发模式枚举
        enum EpollTrigModes
        {
            EPOLL_LT = 0, //水平触发
            EPOLL_ET = 1, //边缘触发
        };

        // mugix核心错误码
        enum CoreErrors
        {
            ERROR_CORE_INIT = 0, //初始化错误
        };

        int epollfd_;                           //epoll 文件描述符
        int mode_epoll_trig_listen_ = EPOLL_LT; //监听套接字触发方式
        int mode_epoll_trig_conn_ = EPOLL_LT;   //连接套接字触发方式
        int server_listen_socketfd_;            //监听套接字
        int server_port_;                       //监听端口

    private:
        ThreadPool pool_; //线程池
    private:
        // epoll相关
        epoll_event events_[k_MAX_EVENT_NUMBER];

        void AddEpollEvent(int fd, int state);
        void ModifyEpollEvent(int fd, int state);
        void DeleteEpollEvent(int fd, int state);

        void SetSocketNonBlock(int fd);
        UserSocket users_[65536];

    private:
        bool Init();
        bool EventListen();
        bool EventLoop();
        bool ProcessNewClient();

    public:
        SockerControler(const int port = 5000) : server_port_(port){};
        SockerControler(const SockerControler &) = delete;
        SockerControler(SockerControler &&) = delete;
        SockerControler &operator=(const SockerControler &) = delete;
        SockerControler &operator=(SockerControler &&) = delete;

        ~SockerControler(){};

    public:
        void run();
    };
}

#endif