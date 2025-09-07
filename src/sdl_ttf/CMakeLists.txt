project(sdl_ttf LANGUAGES CXX VERSION ${PROJECT_VERSION})

if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared SDL3-static)
find_package(SDL3_ttf REQUIRED)

# Private interface for internal headers
add_library(${PROJECT_NAME}-private INTERFACE)
add_library(sdl::${PROJECT_NAME}-private ALIAS ${PROJECT_NAME}-private)
target_include_directories(${PROJECT_NAME}-private INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
)

# Collect source and header files
file( GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
file( GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

# Common configuration for both libraries
if(NOT MSVC)
    set(MATH_LIBRARY m)
else()
    set(MATH_LIBRARY "")
endif()

set(COMMON_LINK_LIBRARIES 
    sdl::${PROJECT_NAME}-private
    SDL3_ttf::SDL3_ttf
    sdl::sdl-core
    sdl::sdl-private
    ${MATH_LIBRARY}
)

if(NOT PROJECT_IS_TOP_LEVEL)
    list(APPEND COMMON_LINK_LIBRARIES 
        standard_compiler_options
        precompiled_headers
    )
endif()

set(COMMON_COMPILE_OPTIONS "")
if(SDLCPP_USE_CLANG_TIDY)
    set(COMMON_COMPILE_OPTIONS CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

# Create static library
add_library(${PROJECT_NAME}-static STATIC)
target_sources(${PROJECT_NAME}-static PRIVATE ${SOURCE_FILES})
target_sources(${PROJECT_NAME}-static
    PUBLIC
        FILE_SET HEADERS 
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include 
            FILES ${HEADER_FILES}
)
target_compile_definitions(${PROJECT_NAME}-static PUBLIC $<UPPER_CASE:${PROJECT_NAME}>_STATIC_DEFINE)
target_link_libraries(${PROJECT_NAME}-static PRIVATE ${COMMON_LINK_LIBRARIES})

# Create shared library  
add_library(${PROJECT_NAME}-shared SHARED)
target_sources(${PROJECT_NAME}-shared PRIVATE ${SOURCE_FILES})
target_sources(${PROJECT_NAME}-shared
    PUBLIC
        FILE_SET HEADERS 
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include 
            FILES ${HEADER_FILES}
)
target_compile_definitions(${PROJECT_NAME}-shared PRIVATE ${PROJECT_NAME}_shared_EXPORTS)
target_link_libraries(${PROJECT_NAME}-shared PRIVATE ${COMMON_LINK_LIBRARIES})

# Set common properties
set_target_properties(${PROJECT_NAME}-static ${PROJECT_NAME}-shared PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-static)
set_target_properties(${PROJECT_NAME}-shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

# Add binary dir to include path for generated headers
target_include_directories(${PROJECT_NAME}-static PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_include_directories(${PROJECT_NAME}-shared PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

# Use specific SDL3 variants for each library type
target_link_libraries(${PROJECT_NAME}-static PRIVATE SDL3::SDL3-static)
target_link_libraries(${PROJECT_NAME}-shared PRIVATE SDL3::SDL3-shared)

# Create aliases
add_library(sdl::${PROJECT_NAME}-shared ALIAS ${PROJECT_NAME}-shared)
add_library(sdl::${PROJECT_NAME}-static ALIAS ${PROJECT_NAME}-static)

# Apply clang-tidy if enabled
if(SDLCPP_USE_CLANG_TIDY)
    set_target_properties(${PROJECT_NAME}-static ${PROJECT_NAME}-shared PROPERTIES CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

# Set up headers for both libraries
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}-shared BASE_NAME ${PROJECT_NAME})

# Add generated export header to both libraries
target_sources(${PROJECT_NAME}-static 
    PUBLIC 
        FILE_SET generated_headers
            TYPE HEADERS
            BASE_DIRS $<TARGET_PROPERTY:${PROJECT_NAME}-shared,BINARY_DIR>
            FILES ${CMAKE_CURRENT_BINARY_DIR}/$<LOWER_CASE:${PROJECT_NAME}>_export.h
)
target_sources(${PROJECT_NAME}-shared 
    PUBLIC 
        FILE_SET generated_headers
            TYPE HEADERS
            BASE_DIRS $<TARGET_PROPERTY:${PROJECT_NAME}-shared,BINARY_DIR>
            FILES ${CMAKE_CURRENT_BINARY_DIR}/$<LOWER_CASE:${PROJECT_NAME}>_export.h
)

# Create default alias pointing to shared (for backward compatibility)
add_library(${PROJECT_NAME} ALIAS ${PROJECT_NAME}-shared)
add_library(sdl::${PROJECT_NAME} ALIAS ${PROJECT_NAME}-shared)

add_library(${PROJECT_NAME}-core INTERFACE)
target_link_libraries(${PROJECT_NAME}-core INTERFACE
    ${PROJECT_NAME}-shared
    ${PROJECT_NAME}-static
)

include(GNUInstallDirs)
# Install core interface library and private headers
install(TARGETS ${PROJECT_NAME}-core ${PROJECT_NAME}-private EXPORT ${TARGET_EXPORT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
)

# Install headers interface library and static/shared libraries with export
install(TARGETS ${PROJECT_NAME}-shared ${PROJECT_NAME}-static EXPORT ${TARGET_EXPORT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${RUNTIME_COMPONENT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${RUNTIME_COMPONENT_NAME} NAMELINK_COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    FILE_SET generated_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
)

if(NOT CMAKE_TESTING_ENABLED)
    include(GoogleTest)
    enable_testing()
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/impl_test)
    
# Include benchmarks if enabled
if(SDLXX_ENABLE_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
