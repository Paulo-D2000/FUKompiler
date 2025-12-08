cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(FUK VERSION 1.0 LANGUAGES CXX)

include_directories(include/)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} main.cpp ${SRCS})

# Unit test target: builds a small test runner that uses the same sources
add_executable(unit_tests tests/test_parser.cpp ${SRCS})