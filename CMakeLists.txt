cmake_minimum_required(VERSION 3.16)

project (mugix)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE source_files ./*.cc)

add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC pthread)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/conf DESTINATION ${CMAKE_SOURCE_DIR}/build/)