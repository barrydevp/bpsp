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

# ANOTHER CMAKE
add_subdirectory(lib)
add_subdirectory(test)

# SOURCE
file(GLOB SERVER_SRC ${PROJECT_SOURCE_DIR}/server/*.c)
file(GLOB TOOLS_SRC ${PROJECT_SOURCE_DIR}/tools/*.c)
file(GLOB TOOLS_SRC ${PROJECT_SOURCE_DIR}/client/c-client/*.c)

# LINK
# link_directories(lib)

# BUILD LIB
# add_library(bpsp STATIC ${LIBS})

# BUILD BIN
add_executable(bpsp_server ${SERVER_SRC})
target_link_libraries(bpsp_server libbpsp)

add_executable(bpsp ${TOOLS_SRC})
target_link_libraries(bpsp libbpsp)

add_executable(client ${TOOLS_SRC})
target_link_libraries(client libbpsp)
