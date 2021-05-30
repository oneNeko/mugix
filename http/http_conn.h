#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string>
#include <map>

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
    void CloseConn(bool real_close = true);
    void Process();

private:
    void Init();
    HTTP_CODE ProcessRead();
    bool ProcessWrite(HTTP_CODE ret);
    HTTP_CODE ParseRequestLine(char *text);
    HTTP_CODE ParseHeaders(char *text);
    HTTP_CODE ParseContent(char *text);
    HTTP_CODE DoRequest();

    bool process_write(HTTP_CODE ret);
    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();

public:
    int m_sockfd;
    sockaddr_in m_address;

public:
    HttpRequest request;
    HttpResponse response;

public:
    string request_text;
    static int m_epollfd;
    static int m_user_count;
};

#endif