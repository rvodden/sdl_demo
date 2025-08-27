cmake_minimum_required(VERSION 3.26.4)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/dynamic_version.cmake)
dynamic_version(PROJECT_PREFIX sdlcpp_)

project(SDL++
    DESCRIPTION "Modern C++20 Wrapper for SDL3"
    LANGUAGES CXX 
    VERSION ${PROJECT_VERSION}
)

include(cmake/project_options.cmake)
include(cmake/standard_build.cmake)
include(cmake/ccache.cmake)
include(cmake/doxygen.cmake)

# Add option to build documentation only
option(SDLXX_DOCS_ONLY "Build documentation only, skip all other targets" OFF)

if(NOT SDLXX_DOCS_ONLY)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(external)
endif()

# Setup documentation generation (doxygen-awesome is handled internally)
setup_doxygen()

if(NOT SDLXX_DOCS_ONLY)
    add_subdirectory(src)
endif()

message("${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_HASH}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")
