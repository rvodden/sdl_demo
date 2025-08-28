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

set(TARGET_EXPORT_NAME ${PROJECT_NAME}Targets)

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

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/
)

install(EXPORT ${TARGET_EXPORT_NAME}
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE sdl::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake 
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
