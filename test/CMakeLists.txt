cmake_minimum_required(VERSION 3.0)

project(mugix_test)

# 设置头文件搜索路径
#include_directories(/home/neko/cpp/mugix/)

SET(CMAKE_BUILD_TYPE "Debug")

file(GLOB_RECURSE source_test_files ./*.cc)
file(GLOB source_files 
    "../src/base/*.cc",
    "../src/http/*.cc",
    "../src/utils/*.cc"
    )

# 生成target文件
add_executable(${PROJECT_NAME}  ${source_files} ${source_test_files})

# 链接gtest库文件
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC gtest pthread mysqlclient)
