cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

INCLUDE_DIRECTORIES("./include")

file(GLOB SOURCES "src/*.cpp")

# Set the project name
project(test)

# Add an executable
add_executable(test ${SOURCES})