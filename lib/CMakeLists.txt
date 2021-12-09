# PROJECT INFO
cmake_minimum_required(VERSION 3.0.0)

# COMPILER
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps ${PROJECT_SOURCE_DIR}/lib)

# INCLUDE
# include_directories(${PROJECT_SOURCE_DIR}/deps)
# include_directories(${PROJECT_SOURCE_DIR}/lib)
# include_directories(${INCLUDE_DIR})

# SOURCE
file(GLOB LIBS ${PROJECT_SOURCE_DIR}/lib/*.c)

# BUILD LIB
add_library(libbpsp STATIC ${LIBS})
target_include_directories(libbpsp PUBLIC ${INCLUDE_DIR})
