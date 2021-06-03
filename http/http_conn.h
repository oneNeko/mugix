#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <string>
#include <unistd.h>
#include "http_define.h"
#include "http_request.h"
#include "http_response.h"

using namespace std;

class HttpConn
{
public:
    HttpConn(){};
    ~HttpConn(){};

public:
    void Init(int sock_fd, const sockaddr_in &addr);
    void ResetConn(bool real_close = true);
    void Process();

private:
    void Init();

    // 处理数据
    HTTP_CODE ProcessRead();
    HTTP_CODE ProcessWrite();
    bool ReadFromSocket();
    bool WriteToSocket();

    // 解析请求数据
    HTTP_CODE ParseRequestLine(char *text);
    HTTP_CODE ParseHeaders(char *text);
    HTTP_CODE ParseContent(char *text);
    HTTP_CODE DoRequest();

public:
    string str_header;
    int rw_state;
    int client_sockfd_;
    sockaddr_in client_address_;
    string request_text_;
    int epoll_trig_mode_;
    static int epollfd_;
    static int user_count_;

    HttpRequest request_;
    HttpResponse response_;
};

#endif