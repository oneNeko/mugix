#include <gtest/gtest.h>
#define private public

#include "../src/http/http_header.h"
using namespace mugix;

#pragma region ParseHeaderMap
TEST(ParseHeaderMap_test, ParseHeaderMap_test_case0)
{
    HttpHeader hh;
    // http请求方式
    ASSERT_EQ(hh.ParseHeaderMap("test", http_methods_), HTTP_METHOD_NOT_SUPPORT);
    ASSERT_EQ(hh.ParseHeaderMap("", http_methods_), HTTP_METHOD_NOT_SUPPORT);
    ASSERT_EQ(hh.ParseHeaderMap("GET", http_methods_), GET);
    ASSERT_EQ(hh.ParseHeaderMap("POST", http_methods_), POST);
    ASSERT_EQ(hh.ParseHeaderMap("HEAD", http_methods_), HEAD);
    // http版本
    ASSERT_EQ(hh.ParseHeaderMap("HTTP/0.9", http_versions_), HTTP_VERSION_NOT_SUPPORT);
    ASSERT_EQ(hh.ParseHeaderMap("HTTP", http_versions_), HTTP_VERSION_NOT_SUPPORT);
    ASSERT_EQ(hh.ParseHeaderMap("HTTP/1.0", http_versions_), HTTP_10);
    ASSERT_EQ(hh.ParseHeaderMap("HTTP/1.1", http_versions_), HTTP_11);
}
#pragma endregion

#pragma region ParseHeader
TEST(ParseHeader_test, ParseHeader_test_case0)
{
    HttpHeader hh;
    // http请求方式
    string header = "GET /index.html HTTP/1.1\r\nHost: blog.oneneko.com\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36\r\n";

    hh.ParseHeader(header);

    ASSERT_EQ(hh.method_, GET);
    ASSERT_EQ(hh.path_, "/index.html");
    ASSERT_EQ(hh.version_, HTTP_11);
}

TEST(ParseHeader_test, ParseHeader_test_case1)
{
    HttpHeader hh;
    // http请求方式
    string header = "TEST /i HTTP\r\nHost: blog.oneneko.com\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36\r\n";

    hh.ParseHeader(header);

    ASSERT_EQ(hh.method_, HTTP_METHOD_NOT_SUPPORT);
    ASSERT_EQ(hh.path_, "");
    ASSERT_EQ(hh.version_, HTTP_VERSION_NOT_SUPPORT);
}

TEST(ParseHeader_test, ParseHeader_test_case2)
{
    HttpHeader hh;
    // http请求方式
    string header = " \r\n";

    hh.ParseHeader(header);

    ASSERT_EQ(hh.method_, HTTP_METHOD_NOT_SUPPORT);
    ASSERT_EQ(hh.path_, "");
    ASSERT_EQ(hh.version_, HTTP_VERSION_NOT_SUPPORT);
}
#pragma endregion

#undef private