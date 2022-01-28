#include <gtest/gtest.h>
#define private public

#include "../src/server/http/http_controler.h"
using namespace mugix::http;

// ParseHeaderMap
TEST(ParseHeaderMap_test, ParseHeaderMap_test_case0)
{
    // http请求方式
    ASSERT_EQ(ParseHeaderMap("test", http_methods_), HTTP_METHOD_NOT_SUPPORT);
    ASSERT_EQ(ParseHeaderMap("", http_methods_), HTTP_METHOD_NOT_SUPPORT);
    ASSERT_EQ(ParseHeaderMap("GET", http_methods_), GET);
    ASSERT_EQ(ParseHeaderMap("POST", http_methods_), POST);
    ASSERT_EQ(ParseHeaderMap("HEAD", http_methods_), HEAD);
    //http版本
    ASSERT_EQ(ParseHeaderMap("HTTP/0.9", http_versions_), HTTP_VERSION_NOT_SUPPORT);
    ASSERT_EQ(ParseHeaderMap("HTTP", http_versions_), HTTP_VERSION_NOT_SUPPORT);
    ASSERT_EQ(ParseHeaderMap("HTTP/1.0", http_versions_), HTTP_10);
    ASSERT_EQ(ParseHeaderMap("HTTP/1.1", http_versions_), HTTP_11);
}

// ParseHeader
TEST(ParseHeader_test, ParseHeader_test_case0)
{
    HttpControler hc;
    // http请求方式
    string header = "GET /index.html HTTP/1.1\r\nHost: blog.oneneko.com\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36\r\n";

    hc.ParseHeader(header);

    ASSERT_EQ(hc.header_.method_, GET);
    ASSERT_EQ(hc.header_.path_, "/index.html");
    ASSERT_EQ(hc.header_.version_, HTTP_11);
}

// ParseHeader
TEST(ParseHeader_test, ParseHeader_test_case1)
{
    HttpControler hc;
    // http请求方式
    string header = "TEST /i HTTP\r\nHost: blog.oneneko.com\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36\r\n";

    hc.ParseHeader(header);

    ASSERT_EQ(hc.header_.method_, HTTP_METHOD_NOT_SUPPORT);
    ASSERT_EQ(hc.header_.path_, "/i");
    ASSERT_EQ(hc.header_.version_, HTTP_VERSION_NOT_SUPPORT);
}

#undef private