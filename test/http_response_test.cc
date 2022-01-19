#include <gtest/gtest.h>

#define private public
#include "../server/http/http_response.h"
#include "../server/config/config.h"

// CheckFile
/*TEST(CheckFile_test, CheckFile_test_case0)
{
    HttpResponse response;
    response.file_path_ = "/403.txt";
    auto config = Config::GetInstance();
    config->DIR_PATH = "/home/neko/cpp/mugix/mugix_src/html/";
    response.CheckFile();

    ASSERT_EQ(response.response_code_, 403);
}

TEST(CheckFile_test, CheckFile_test_case1)
{
    HttpResponse response;
    auto config = Config::GetInstance();
    config->DIR_PATH = "/home/neko/cpp/mugix/mugix_src/html/";
    response.file_path_ = "index.html";

    response.CheckFile();

    ASSERT_EQ(response.response_code_, 200);
    ASSERT_EQ(response.content_length_, 215);
}

TEST(CheckFile_test, CheckFile_test_case2)
{
    HttpResponse response;
    response.file_path_ = "html/";
    auto config = Config::GetInstance();
    config->DIR_PATH = "/home/neko/cpp/mugix/mugix_src/html/";

    response.CheckFile();

    ASSERT_EQ(response.response_code_, 404);
}

TEST(CheckFile_test, CheckFile_test_case3)
{
    HttpResponse response;
    response.file_path_ = "html";
    auto config = Config::GetInstance();
    config->DIR_PATH = "/home/neko/cpp/mugix/mugix_src/html/";

    response.CheckFile();

    ASSERT_EQ(response.response_code_, 404);
}

TEST(CheckFile_test, CheckFile_test_case4)
{
    HttpResponse response;
    auto config = Config::GetInstance();
    config->DIR_PATH = "/home/neko/cpp/mugix/mugix_src/html/";
    response.file_path_ = "";

    response.CheckFile();
    // 路径空代表是首页
    ASSERT_EQ(response.response_code_, 200);
}
*/

#undef private