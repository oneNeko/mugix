cmake_minimum_required(VERSION 3.16)

project (mugix)

set(CMAKE_CXX_STANDARD 11)

file(GLOB source_files "utils/*.cc" "config/*.cc" "log/*.cc" "server/*.cc" "routes/*.cc" "*.cc")
add_executable(${PROJECT_NAME} ${source_files})

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/conf DESTINATION ${CMAKE_SOURCE_DIR}/build/)