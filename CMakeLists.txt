cmake_minimum_required(VERSION 3.10)
project(jxl)

# 包含头文件
include_directories(./jsoncpp/include ./*.h)
set(jsoncpp jsoncpp/json_reader.cpp jsoncpp/json_writer.cpp jsoncpp/json_value.cpp)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

# 添加可执行代码
add_executable(jxl main.cpp ${jsoncpp} SrvInfo.h SrvInfo.cpp utils.h SrvInfoUdpServer.h SrvInfoUdpServer.cpp)

# 在add_executable之后添加link_libraries
target_link_libraries (${PROJECT_NAME} pthread)