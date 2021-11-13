cmake_minimum_required(VERSION 3.0.0)
project(bpsp VERSION 1.0.0)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

include(CTest)
enable_testing()

include_directories(test)
link_directories(lib)

file(GLOB SOURCES src/*.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
