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
include(cmake/iwyu.cmake)
include(cmake/coverage.cmake)

# Setup code coverage first (must be before targets are defined)
setup_coverage()

# Add option to build documentation only

if(NOT SDLXX_DOCS_ONLY)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(external)
    add_subdirectory(src)
    
    # Enable IWYU for all project targets after they've been defined
    if(SDLXX_USE_IWYU)
        enable_iwyu_for_project_targets()
    endif()
    
    # Enable coverage for all project targets after they've been defined
    if(SDLXX_ENABLE_COVERAGE)
        enable_coverage_for_project_targets()
    endif()
endif()

# Setup documentation generation (doxygen-awesome is handled internally)
setup_doxygen()

message("${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_HASH}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")
