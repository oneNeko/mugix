#include <string>
#include <unordered_map>
#include <regex>
#include <iostream>

#include <sys/socket.h>

class HttpServer
{
public:
    HttpServer();
    ~HttpServer();

    bool Start(int port);
    void Run();
    void Stop();

private:
    int server_listen_socket;
    int server_port;
    bool is_run;

    bool Init();

    //处理请求
    void ChildSocketProcess(int socket_fd);
};

/* 
GET /bing HTTP/1.0
Host: 127.0.0.1:50001
Connection: close
Cache-Control: max-age=0
DNT: 1
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36 Edg/90.0.818.62
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,;q=0.8,application/signed-exchange;v=b3;q=0.9
Accept-Encoding: gzip, deflate
Accept-Language: zh-CN,zh;q=0.9 
*/
/*
HTTP/1.1 200 OK
Server: nginx/1.18.0 (Ubuntu)
Date: Thu, 20 May 2021 06:24:08 GMT
Content-Type: text/html; charset=utf-8
Transfer-Encoding: chunked
Connection: keep-alive
Vary: Cookie
Set-Cookie: session=eyJfZnJlc2giOmZhbHNlLCJjc3JmX3Rva2VuIjoiN2ZjYWFmMzM2MjI3MDEwNzUxN2M1NjY3MzYyNWQ0NDYwMTBmODNhZCJ9.YKYAiA.1ySQXoyDnUyNRm_-t0Gmm0tFbpA; HttpOnly; Path=/
Content-Encoding: gzip
*/