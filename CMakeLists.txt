cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(FUK VERSION 1.0 LANGUAGES CXX)

include_directories(include/)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} main.cpp ${SRCS})