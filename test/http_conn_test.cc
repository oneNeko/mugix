#include <gtest/gtest.h>

#define private public
#include "../server/http/http_conn.h"

// process
TEST(Process_test, Process_test_case0)
{
    string text = "POST / HTTP/1.1\r\nHost: 127.0.0.1:50001\r\nConnection: keep-alive\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36 Edg/90.0.818.66\r\n\r\n";
    HttpConn conn;
    conn.request_text_=text;
    vector<string> res={"GET","/","HTTP/1.1"};
    ASSERT_EQ(1, conn.ProcessRead());
}

// ProcessRead
TEST(ProcessRead_test, ProcessRead_test_case0)
{
    string text = "GET / HTTP/1.1\r\nHost: 127.0.0.1:50001\r\nConnection: keep-alive\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36 Edg/90.0.818.66\r\n\r\n";
    HttpConn conn;
    conn.request_text_=text;
    vector<string> res={"GET","/","HTTP/1.1"};
    ASSERT_EQ(1, conn.ProcessRead());
}

TEST(ProcessRead_test, ProcessRead_test_case1)
{
    string text = "POST /upload HTTP/1.1\r\nUser-Agent: PostmanRuntime/7.28.0\r\nAccept: */*\r\nPostman-Token: e1c404c5-f21f-44d7-9274-6d552c798ab9\r\nHost: 192.168.9.254:50001\r\nAccept-Encoding: gzip, deflate, br\r\nConnection: keep-alive\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 20\r\n\r\nid=neko&password=123";
    
    HttpConn conn;
    conn.request_text_=text;
    
    ASSERT_EQ(1, conn.ProcessRead());
    ASSERT_EQ(POST,conn.request_.method_);
    ASSERT_EQ("/upload",conn.request_.abs_path_);
    ASSERT_EQ("HTTP/1.1",conn.request_.http_protocol_);
    ASSERT_EQ("20",conn.request_.header_params_["Content-Length"]);
}



#undef private