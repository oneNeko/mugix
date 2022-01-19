#include <gtest/gtest.h>

#define private public
#include "../server/http/http_request.h"
#include "../server/utils/utils.h"

// SplitString
TEST(SplitString_test, SplitString_test_case0)
{
    string text = "GET / HTTP/1.1";
    vector<string> res={"GET","/","HTTP/1.1"};
    ASSERT_EQ(res, Utils::SplitString(text," "));
}

TEST(SplitString_test, SplitString_test_case1)
{
    string text = "GET  ";
    vector<string> res={"GET"};
    ASSERT_EQ(res, Utils::SplitString(text,"  "));
}

// ParseRequestLine
TEST(ParseRequestLine_test, ParseRequestLine_test_case0)
{
    string text = "GET / HTTP/1.1";
    HttpRequest request;
    ASSERT_EQ(0, request.ParseRequestLine(text));
    ASSERT_EQ(request.method_, GET);
    ASSERT_EQ(request.abs_path_, "/");
    ASSERT_EQ(request.http_protocol_, "HTTP/1.1");
}

TEST(ParseRequestLine_test, ParseRequestLine_test_case1)
{
    string text = "GET /?ma HTTP/1.1";
    HttpRequest request;
    ASSERT_EQ(10, request.ParseRequestLine(text));
    ASSERT_EQ(request.method_, GET);
    ASSERT_EQ(request.abs_path_, "/");
}

// ParseUriParams
TEST(ParseUriParams_test, ParseUriParams_test_case0)
{
    string text = "/";
    HttpRequest request;
    ASSERT_EQ(0, request.ParseUriParams(text));
    ASSERT_EQ(request.abs_path_, "/");
}

TEST(ParseUriParams_test, ParseUriParams_test_case1)
{
    string text = "/?id=neko&query=book";
    HttpRequest request;
    ASSERT_EQ(0, request.ParseUriParams(text));
    ASSERT_EQ(request.abs_path_, "/");
    ASSERT_EQ(request.uri_params_["id"], "neko");
    ASSERT_EQ(request.uri_params_["query"], "book");
}

TEST(ParseUriParams_test, ParseUriParams_test_case2)
{
    string text = "/?id=nekoquery=book";
    HttpRequest request;
    ASSERT_EQ(-1, request.ParseUriParams(text));
}

//ParseHeader
TEST(ParseHeader, ParseHeader_test_case0)
{
    string text = "GET / HTTP/1.1\r\nHost: 127.0.0.1:50001\r\nConnection: keep-alive\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36 Edg/90.0.818.66";
    HttpRequest request;
    ASSERT_EQ(GET_REQUEST, request.ParseHeader(text));
    ASSERT_EQ(request.header_params_["Host"], "127.0.0.1:50001");
}

TEST(ParseHeader, ParseHeader_test_case1)
{
    string text = "GET / HTTP/1.1\r\nHost: 127.0.0.1:50001\r\nConnection: keep-alive\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36 Edg/90.0.818.66\r\n";
    HttpRequest request;
    ASSERT_EQ(GET_REQUEST, request.ParseHeader(text));
    ASSERT_EQ(request.header_params_["Host"], "127.0.0.1:50001");
}

TEST(ParseHeader, ParseHeader_test_case2)
{
    string text = "GET /index.html? HTTP/1.1\r\nHost: 127.0.0.1:50001\r\nConnection: keep-alive\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36 Edg/90.0.818.66\r\n";
    HttpRequest request;
    ASSERT_EQ(GET_REQUEST, request.ParseHeader(text));
    ASSERT_EQ(request.abs_path_, "/index.html");
    ASSERT_EQ(request.header_params_["Host"], "127.0.0.1:50001");
}

TEST(ParseHeader, ParseHeader_test_case3)
{
    string text = "GET /404 HTTP/1.0\r\nUser-Agent: WebBench 1.5\r\nHost: 127.0.0.1";
    HttpRequest request;
    ASSERT_EQ(GET_REQUEST, request.ParseHeader(text));
    ASSERT_EQ(request.abs_path_, "/404");
    ASSERT_EQ(request.header_params_["Host"], "127.0.0.1");
}

#undef private