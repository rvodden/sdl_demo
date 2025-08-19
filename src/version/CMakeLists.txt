project(sdl++_version)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE FILE_SET HEADERS FILES ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# Make sure version is re-calculated even if you pass `cmake --build . --target version_lib`
# `My_Project_Version` is automatically generated target with the name from `PROJECT_PREFIX`
add_dependencies(${PROJECT_NAME} sdlcpp_version)

# Rebuild `version.cpp` whenever the version changes
# `.version` is automatically generated
set_property(SOURCE version.h.in APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/.version)
