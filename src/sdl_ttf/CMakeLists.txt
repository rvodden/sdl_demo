project(sdl_ttf LANGUAGES CXX VERSION ${PROJECT_VERSION})

if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared SD3-static)
find_package(SDL3_image REQUIRED)

# Private interface for internal headers
add_library(${PROJECT_NAME}_private INTERFACE)
target_include_directories(${PROJECT_NAME}_private INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
)

# Collect source and header files
file( GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
file( GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

# Create object libraries for static and shared builds
add_library(${PROJECT_NAME}-objects OBJECT)
target_sources(${PROJECT_NAME}-objects PRIVATE ${SOURCE_FILES})
target_sources(${PROJECT_NAME}-objects
    PUBLIC
        FILE_SET HEADERS 
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include 
            FILES ${HEADER_FILES}
)
target_link_libraries(${PROJECT_NAME}-objects PRIVATE ${PROJECT_NAME}_private)
target_link_libraries(${PROJECT_NAME}-objects PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME}-objects PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(${PROJECT_NAME}-objects PUBLIC  sdl::sdl-core)

if(NOT MSVC)
    target_link_libraries(${PROJECT_NAME}-objects PUBLIC m)
endif()

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME}-objects PRIVATE standard_compiler_options)
    target_link_libraries(${PROJECT_NAME}-objects PRIVATE precompiled_headers)
endif()

if(SDLCPP_USE_CLANG_TIDY)
    set_target_properties(${PROJECT_NAME}-objects PROPERTIES CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

add_library(${PROJECT_NAME}-static STATIC)


add_library(${PROJECT_NAME}-shared SHARED)


set(SUFFIXES -shared -static)
set(${PUBLIC_DEPENDENCIES} sdl::sdl)
foreach(SUFFIX ${SUFFIXES})
    set(LIBRARY ${PROJECT_NAME}${SUFFIX})
    target_sources(${LIBRARY} PRIVATE $<TARGET_OBJECTS:${PROJECT_NAME}-objects>)
    target_include_directories(${LIBRARY} PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )
    set_target_properties(${LIBRARY} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME ${PROJECT_NAME}
    )
    foreach(DEPENDENCY ${PUBLIC_DEPENDENCIES})
        target_link_libraries(${LIBRARY} PUBLIC ${DEPENDENCY}-${SUFFIX})
    endforeach()
endforeach()

add_library(sdl::${PROJECT_NAME}-shared ALIAS ${PROJECT_NAME}-shared)
set_target_properties(${PROJECT_NAME}-shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
add_library(sdl::${PROJECT_NAME}-static ALIAS ${PROJECT_NAME}-static)
set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}-shared PUBLIC SDL_EXPORTS)
target_compile_definitions(${PROJECT_NAME}-static PUBLIC SDL_STATIC)

# Set up headers for both libraries
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}-shared BASE_NAME ${PROJECT_NAME})

target_sources(${PROJECT_NAME}-objects 
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
# Install object library separately (not exported since it's an implementation detail)
install(TARGETS ${PROJECT_NAME}-objects
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    FILE_SET generated_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
)

# Install headers interface library and static/shared libraries with export
install(TARGETS ${PROJECT_NAME}-shared ${PROJECT_NAME}-static EXPORT ${TARGET_EXPORT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${RUNTIME_COMPONENT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${RUNTIME_COMPONENT_NAME} NAMELINK_COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
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
