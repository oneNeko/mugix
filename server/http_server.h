#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
#endif

#include <string>
#include <unordered_map>
#include <regex>
#include <iostream>

#include <sys/socket.h>

#include "../http/http_conn.h"
#include "../http/http_define.h"

const int k_MAX_FD = 65536;
const int k_MAX_EVENT_NUMBER = 10000;
const int k_MAX_EPOLL = 1000;

class HttpServer
{
public:
    int m_epoll_fd;
    HttpConn *users;

public:
    HttpServer();
    ~HttpServer();

    void Init();

private:
    int server_listen_socket;
    int server_port;
    bool is_run;

public:
    int EventListen();
    void EventLoop();
    bool dealclientdata(int sockfd);
    void dealwithread(int sockfd);
    void dealwithwrite(int sockfd);

private:
    int m_listen_fd;
    epoll_event events[k_MAX_EVENT_NUMBER];
};