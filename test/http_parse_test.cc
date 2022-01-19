/*#include <gtest/gtest.h>

#define private public
#include "../mugix_src/http/http_parse.h"

using namespace HTTP;

// ParseRequestLine
TEST(ParseRequestLine1_test, ParseRequestLine1_test_case0)
{
    string line = "GET / HTTP/1.1";
    HttpParse http_parse;

    auto res = http_parse.ParseRequestLine(line);

    ASSERT_EQ(res, LINE_PENDING);
    ASSERT_EQ(http_parse.request_method_, GET);
    ASSERT_EQ(http_parse.request_abs_path_, "/");
    ASSERT_EQ(http_parse.request_http_protocol_, "HTTP/1.1");
}

TEST(ParseRequestLine1_test, ParseRequestLine1_test_case1)
{
    string line = "POST /index.html HTTP/1.0";
    HttpParse http_parse;

    auto res = http_parse.ParseRequestLine(line);

    ASSERT_EQ(res, LINE_PENDING);
    ASSERT_EQ(http_parse.request_method_, POST);
    ASSERT_EQ(http_parse.request_abs_path_, "/index.html");
    ASSERT_EQ(http_parse.request_http_protocol_, "HTTP/1.0");
}

TEST(ParseRequestLine1_test, ParseRequestLine1_test_case2)
{
    string line = "GET / HTTP/1.1 ";
    HttpParse http_parse;

    auto res = http_parse.ParseRequestLine(line);

    ASSERT_EQ(res, LINE_ERROR);
}

TEST(ParseRequestLine1_test, ParseRequestLine1_test_case3)
{
    string line = "sfasdfas";
    HttpParse http_parse;

    auto res = http_parse.ParseRequestLine(line);

    ASSERT_EQ(res, LINE_ERROR);
}

// ParseRequestHeaderParams
TEST(ParseRequestHeaderParams_test, ParseRequestHeaderParams_test_case0)
{
    string line = "sfasdfas";
    HttpParse http_parse;

    auto res = http_parse.ParseRequestHeaderParams(line);

    ASSERT_EQ(res, LINE_ERROR);
}

TEST(ParseRequestHeaderParams_test, ParseRequestHeaderParams_test_case1)
{
    string line = "Host: oneneko.com";
    HttpParse http_parse;

    auto res = http_parse.ParseRequestHeaderParams(line);

    ASSERT_EQ(res, LINE_PENDING);
    ASSERT_EQ(http_parse.request_host_, "oneneko.com");
}

TEST(ParseRequestHeaderParams_test, ParseRequestHeaderParams_test_case2)
{
    string line = "Hostoneneko.com: ";
    HttpParse http_parse;

    auto res = http_parse.ParseRequestHeaderParams(line);

    ASSERT_EQ(res, LINE_ERROR);
}

TEST(ParseRequestHeaderParams_test, ParseRequestHeaderParams_test_case3)
{
    string line = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.77 Safari/537.36 Edg/91.0.864.41";
    HttpParse http_parse;

    auto res = http_parse.ParseRequestHeaderParams(line);

    ASSERT_EQ(res, LINE_PENDING);
    ASSERT_EQ(http_parse.request_user_agent_, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.77 Safari/537.36 Edg/91.0.864.41");
}

// ParseRequest
TEST(ParseRequest_test, ParseRequest_test_case0)
{
    string request = "GET / HTTP/1.1\r\nHost: oneneko.com\r\nConnection: keep-alive\r\nsec-ch-ua: \" Not;ABrand\"v=\"99\", \"Microsoft Edge\";v=\"91\", \"Chromium\";v=\"91\"\r\nsec-ch-ua-mobile: ?0\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.77 Safari/537.36 Edg/91.0.864.41\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document";
    HttpParse http_parse;

    auto res = http_parse.ParseRequest(request);

    ASSERT_EQ(res, GET_REQUEST);
    ASSERT_EQ(http_parse.request_method_, GET);
    ASSERT_EQ(http_parse.request_abs_path_, "/");
    ASSERT_EQ(http_parse.request_http_protocol_, "HTTP/1.1");
    ASSERT_EQ(http_parse.request_host_, "oneneko.com");
    ASSERT_EQ(http_parse.request_connection_, "keep-alive");
    ASSERT_EQ(http_parse.request_user_agent_, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.77 Safari/537.36 Edg/91.0.864.41");
}

#undef private
*/