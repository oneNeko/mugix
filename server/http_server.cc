#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <error.h>
#include <errno.h>
#include <stdlib.h>

#include "http_server.h"
#include "Signal.h"
#include "sigchildwait.h"

#include "../log/log.h"
#include "../routes/routes.h"


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

    void sig_chld(int);
    Signal(SIGCHLD, sig_chld);

    while (is_run)
    {
        sockaddr_in addr_client;
        socklen_t addr_client_len;
        connfd = accept(server_listen_socket, (sockaddr *)&addr_client, &addr_client_len);
        if (errno == EINTR)
        {
            continue;
        }
        if ((child_pid = fork()) == 0)
        {
            close(server_listen_socket);
            ChildSocketProcess(connfd);
            exit(0);
        }
        close(connfd);
    }
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

    return true;
}

void HttpServer::ChildSocketProcess(int socket_fd)
{
    int res = 0;
    do
    {
        char buf[1024 * 2] = {0};
        res = recv(socket_fd, buf, sizeof(buf) - 1, 0);
        if (res == 0)
        {
            break;
        }
        buf[res] = '\0';
        Log(buf);

        //处理http请求
        ROUTES route;
        auto response_text=route.process_requests(buf);

        //响应http请求
        send(socket_fd, response_text.c_str(), response_text.size(), 0);

        break;

    } while (res != 0 && res != -1);
    close(socket_fd);
}