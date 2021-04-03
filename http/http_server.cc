#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <error.h>
#include <errno.h>
#include <stdlib.h>

#include "http_server.h"
#include "../log/log.h"
#include "Signal.h"
#include "sigchildwait.h"

#include <iostream>
using namespace std;

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
    Log("Init finshed");
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
        std::string response_str = ProcessResponse("hello world!");
        send(socket_fd, response_str.c_str(), response_str.size(), 0);
        break;

    } while (res != 0 && res != -1);
    close(socket_fd);
}

std::string HttpServer::ProcessRequest()
{
    return "";
}

std::string HttpServer::ProcessResponse(std::string response)
{
    std::string rmsg = "";
    rmsg = "HTTP/1.1 200 OK\r\n";
    rmsg += "Server: xHttp\r\n";
    rmsg += "Content-Type: text/html;charset=utf-8\r\n";
    rmsg += "Content-Length: ";
    rmsg += std::to_string(response.size());
    //rmsg += "\r\n";
    rmsg += "\r\n\r\n";
    rmsg += response;
    return rmsg;
}