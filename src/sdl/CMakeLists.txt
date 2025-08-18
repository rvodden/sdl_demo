project(sdl LANGUAGES CXX)
add_library(${PROJECT_NAME} SHARED)

file( GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
file( GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

include(CMakePrintHelpers)
cmake_print_variables(HEADER_FILES)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include FILES ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE src/include/)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)
target_link_libraries(${PROJECT_NAME} PRIVATE utils)

target_link_libraries(${PROJECT_NAME} PUBLIC m)

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME} PRIVATE standard_compiler_options)
endif()

if(NOT CMAKE_TESTING_ENABLED)
    include(GoogleTest)
    enable_testing()
endif()
    
set( TEST_NAME "${PROJECT_NAME}_test" )

file( GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/*.h )
foreach( SOURCE_FILE ${TEST_SOURCE_FILES} )
    message( "   Adding test source file: ${SOURCE_FILE}" )
endforeach()
add_executable( ${TEST_NAME} ${TEST_SOURCE_FILES} )

target_link_libraries( ${TEST_NAME} PRIVATE standard_compiler_options )
target_link_libraries( ${TEST_NAME} PUBLIC ${PROJECT_NAME} )
target_link_libraries( ${TEST_NAME} PUBLIC gtest )
target_link_libraries( ${TEST_NAME} PUBLIC gmock )
target_link_libraries( ${TEST_NAME} PUBLIC gtest_main )

gtest_discover_tests( ${TEST_NAME} XML_OUTPUT_DIR report EXTRA_ARGS --gtest_catch_exceptions=0 )

set(PROJECT_NAME ${PROJECT_NAME} PARENT_SCOPE)
