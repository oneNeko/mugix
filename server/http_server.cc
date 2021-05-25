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

#include "http_server.h"

#include "../log/log.h"
#include "../routes/routes.h"
#include "mgx_epoll.h"

HttpServer::HttpServer() : server_port(50001), is_run(false)
{
}

HttpServer::~HttpServer()
{
}

bool HttpServer::Start(int port)
{
    server_port = port;

    if (!Init())
    {
        Log("http server init failed");
        return false;
    }
    Log("Init successed");
    return true;
}

void HttpServer::Run()
{
    pid_t child_pid;
    int connfd;

    if (is_run)
    {
        Log("http server is running");
        return;
    }

    is_run = true;

    MgxEpoll::do_epoll(server_listen_socket);
}

void HttpServer::Stop()
{
}

bool HttpServer::Init()
{

    server_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_listen_socket == -1)
    {
        Log("socket init error");
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(server_listen_socket, (sockaddr *)&addr, sizeof(addr));
    if (ret != 0)
    {
        Log("Bind error!");
        return false;
    }

    ret = listen(server_listen_socket, 1000);
    if (ret == 0)
    {
        Log("listening at 0.0.0.0:" + std::to_string(server_port));
    }
    else
    {
        Log("listen failed");
        return false;
    }

    return true;
}