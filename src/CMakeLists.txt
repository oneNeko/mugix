cmake_minimum_required(VERSION 3.16)

project (mugix)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")

file(GLOB_RECURSE source_files ./*.cc)

add_executable(${PROJECT_NAME} ${source_files})

# 链接库文件
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC pthread)

# 指定目录
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/conf DESTINATION ${CMAKE_SOURCE_DIR}/build/)