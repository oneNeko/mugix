#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
#endif

#include <string>
#include <sys/socket.h>

#include "../http/http_conn.h"
#include "../http/http_define.h"
#include "../log/log.h"
#include "../threadpool/thread_pool.h"

const int k_MAX_FD = 65536;
const int k_MAX_EVENT_NUMBER = 10000;
const int k_MAX_EPOLL = 1000;

class HttpServer
{
private:
    enum EPOLL_TRIG_MODE
    {
        LT = 0,
        ET = EPOLLET
    };

    int epollfd_;
    int epoll_trig_mode_listen_ = LT;
    int epoll_trig_mode_conn_ = LT;
    int server_listen_socketfd_;
    int server_port_;

    HttpConn *users_;
    epoll_event events_[k_MAX_EVENT_NUMBER];

    //线程池相关
    ThreadPool<HttpConn> *pool_;
    int thread_num_ = 10;
    int max_requests_ = 10000;

    //日志
    Log* logger;

private:
    void InitThreadPool();

    // 处理connect socket事件
    bool ProcessNewClient(int sockfd);
    void ProcessRead(int sockfd);
    void ProcessWrite(int sockfd);

public:
    HttpServer();
    ~HttpServer();

    void Init();
    int EventListen();
    void EventLoop();
};