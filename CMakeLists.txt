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

# LINK
# link_directories(lib)

# BUILD LIB
# add_library(bpsp STATIC ${LIBS})

# BUILD BIN
add_executable(server ${SERVER_SRC})
target_link_libraries(server bpsp)

add_executable(bpsp_tools ${TOOLS_SRC})
target_link_libraries(bpsp_tools bpsp)

