cmake_minimum_required(VERSION 3.26.4)

set(CMAKE_TOOLCHAIN_FILE /CMakeToolchain.txt)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/standard_build.cmake)
project(test_project LANGUAGES CXX)

add_subdirectory(data)
add_subdirectory(external)

file(GLOB sources_list LIST_DIRECTORIES true src/*)
foreach(dir ${sources_list})
    if(IS_DIRECTORY ${dir} AND EXISTS "${dir}/CMakeLists.txt")
        add_subdirectory(${dir})
    endif()
endforeach()
