project(sdl LANGUAGES CXX VERSION ${PROJECT_VERSION})

if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED)

add_library(${PROJECT_NAME}_private INTERFACE)
target_include_directories(${PROJECT_NAME}_private INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
)

add_library(${PROJECT_NAME} SHARED)
add_library(sdl::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}) # public export header

file( GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
file( GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
target_sources(${PROJECT_NAME} 
    PUBLIC 
        FILE_SET HEADERS 
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include 
            FILES ${HEADER_FILES}
        FILE_SET generated_headers
            TYPE HEADERS
            BASE_DIRS $<TARGET_PROPERTY:${PROJECT_NAME},BINARY_DIR>
            FILES ${CMAKE_CURRENT_BINARY_DIR}/$<LOWER_CASE:${PROJECT_NAME}>_export.h
    )

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_private)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)

if(NOT MSVC)
    target_link_libraries(${PROJECT_NAME} PUBLIC m)
endif()

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME} PRIVATE standard_compiler_options)
    target_link_libraries(${PROJECT_NAME} PRIVATE precompiled_headers)
endif()

if(SDLCPP_USE_CLANG_TIDY)
  set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()


include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} EXPORT ${TARGET_EXPORT_NAME}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    FILE_SET generated_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
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

