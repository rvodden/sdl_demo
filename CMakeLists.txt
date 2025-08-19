cmake_minimum_required(VERSION 3.26.4)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/dynamic_version.cmake)
dynamic_version(PROJECT_PREFIX sdlcpp_)

project(sdl++ 
    LANGUAGES CXX 
    VERSION ${PROJECT_VERSION}
)

include(cmake/project_options.cmake)
include(cmake/standard_build.cmake)
include(cmake/ccache.cmake)

add_subdirectory(external)
add_subdirectory(src)

message("${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_HASH}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")
