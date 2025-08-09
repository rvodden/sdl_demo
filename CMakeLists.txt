cmake_minimum_required(VERSION 3.26.4)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(cmake/standard_build.cmake)
project(test_project LANGUAGES CXX)

set(CMAKE_CXX_CLANG_TIDY clang-tidy)

add_subdirectory(external)
add_subdirectory(src)

