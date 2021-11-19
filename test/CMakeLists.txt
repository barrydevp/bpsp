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

add_executable(test_topic test_topic.c)
target_link_libraries(test_topic bpsp)
add_test(topic test_topic)
