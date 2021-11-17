# PROJECT INFO
cmake_minimum_required(VERSION 3.0.0)
project(bpsp VERSION 1.0.0)

# COMPILER
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# TEST SUITE
add_executable(test_frame test_frame.c)
target_link_libraries(test_frame bpsp)
add_test(frame test_frame)

