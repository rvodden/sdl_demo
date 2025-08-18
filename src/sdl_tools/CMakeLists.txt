project(sdl_tools LANGUAGES CXX)
add_library(${PROJECT_NAME} SHARED)

file( GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
file( GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

include(CMakePrintHelpers)
cmake_print_variables(HEADER_FILES)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include FILES ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE src/include/)

target_link_libraries(${PROJECT_NAME} PUBLIC sdl)

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME} PRIVATE standard_compiler_options)
endif()
