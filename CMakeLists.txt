cmake_minimum_required(VERSION 3.26)
project(
    ccwc
    VERSION 1.0
    LANGUAGES CXX
)

if (NOT DEFINED CCWC_MASTER_PROJECT)
  set(FMT_MASTER_PROJECT OFF)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CCWC_MASTER_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
  endif ()
endif ()

set(CMAKE_CXX_STANDARD 23)

option(CCWC_CCACHE_BUILD "Use ccache" ${CCWC_MASTER_PROJECT})

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

option(CCWC_TEST "Generate the test target." ${CCWC_MASTER_PROJECT})

if (CCWC_TEST)
    add_subdirectory(test)
endif ()

add_executable(${PROJECT_NAME} ccwc/main.cpp)