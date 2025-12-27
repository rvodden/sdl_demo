cmake_minimum_required(VERSION 3.26.4)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(cmake/dynamic_version.cmake)
dynamic_version(PROJECT_PREFIX sdlcpp_)

project(SDLXX
    DESCRIPTION "Modern C++20 Wrapper for SDL3"
    LANGUAGES CXX 
    VERSION ${PROJECT_VERSION}
)

set(TARGET_EXPORT_NAME ${PROJECT_NAME}Targets)
set(DEVELOPMENT_COMPONENT_NAME ${PROJECT_NAME}_Development)
set(RUNTIME_COMPONENT_NAME ${PROJECT_NAME}_Runtime)
set(PACKAGE_NAME SDLXX)

include(project_options)
include(standard_build)
include(ccache)
include(doxygen)
include(iwyu)
include(coverage)
include(wrap_cmake_find_package)

# Setup code coverage first (must be before targets are defined)
setup_coverage()

# Add option to build documentation only

if(NOT SDLXX_DOCS_ONLY)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(external)
    add_subdirectory(src)
    add_subdirectory(cmake/test)
    
    # Enable IWYU for all project targets after they've been defined
    if(SDLXX_USE_IWYU)
        enable_iwyu_for_project_targets()
    endif()
endif()

# Setup documentation generation (doxygen-awesome is handled internally)
setup_doxygen()

message("${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_HASH}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${PACKAGE_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PACKAGE_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}/
)

install(EXPORT ${TARGET_EXPORT_NAME}
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE sdl::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}
    COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
)

install(FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake 
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}
    COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
)
