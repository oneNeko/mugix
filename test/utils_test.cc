#include <gtest/gtest.h>
#define private public

#include <string>

#include "../src/utils/utils.h"

TEST(UrlEncode_test, UrlEncode_test_case0)
{
    // 测试加号
    std::string source="C++-%E7%BA%BF%E7%A8%8B%E6%B1%A0.html";
    std::string target="C++-线程池.html";

    ASSERT_EQ(target,UrlDecode(source));
}

TEST(UrlEncode_test, UrlEncode_test_case1)
{
    std::string source="%E5%B7%A6%E5%80%BC%E4%B8%8E%E5%8F%B3%E5%80%BC";
    std::string target="左值与右值";

    ASSERT_EQ(target,UrlDecode(source));
}
