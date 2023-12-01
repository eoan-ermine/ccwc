cmake_minimum_required(VERSION 3.26)
project(ccwc)

set(CMAKE_CXX_STANDARD 23)

find_package(fmt CONFIG REQUIRED)

add_executable(ccwc main.cpp)
target_link_libraries(ccwc PRIVATE fmt::fmt)
