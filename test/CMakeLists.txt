# COMPILER
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# TEST SUITE
add_executable(frame_test frame_test.c)
target_link_libraries(frame_test libbpsp)
add_test(frame frame_test)

add_executable(topic_test topic_test.c)
target_link_libraries(topic_test libbpsp)
add_test(topic topic_test)

add_executable(client_test client_test.c)
target_link_libraries(client_test libbpsp)
add_test(client client_test)

add_executable(util_test util_test.c)
target_link_libraries(util_test libbpsp)
add_test(util util_test)
