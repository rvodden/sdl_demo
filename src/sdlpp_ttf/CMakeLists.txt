project(sdlpp_ttf LANGUAGES CXX)

add_library(${PROJECT_NAME}_private INTERFACE)
target_include_directories(${PROJECT_NAME}_private INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
)

add_library(${PROJECT_NAME} SHARED)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

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

target_link_libraries(${PROJECT_NAME} PRIVATE sdlpp_ttf_private)
target_link_libraries(${PROJECT_NAME} PRIVATE sdlpp_private)
target_link_libraries(${PROJECT_NAME} PRIVATE sdlpp)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf)

if(NOT MSVC)
    target_link_libraries(${PROJECT_NAME} PUBLIC m)
endif()

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME} PRIVATE standard_compiler_options)
    target_link_libraries(${PROJECT_NAME} PRIVATE precompiled_headers)
endif()

if(SDLCPP_USE_CLANG_TIDY)
  set_target_properties(standard_compiler_options PROPERTIES CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

if(NOT CMAKE_TESTING_ENABLED)
    include(GoogleTest)
    enable_testing()
endif()
    
set( TEST_NAME "${PROJECT_NAME}_test" )

file( GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/*.h )
add_executable( ${TEST_NAME} ${TEST_SOURCE_FILES} )

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${TEST_NAME} PRIVATE standard_compiler_options)

    if(SDLCPP_USE_PRECOMPILED_HEADERS)
        target_link_libraries(${TEST_NAME} PRIVATE precompiled_headers)
    endif()
endif()

# Disable clang-tidy for test files
set_target_properties( ${TEST_NAME} PROPERTIES CXX_CLANG_TIDY "" )

target_link_libraries( ${TEST_NAME} PUBLIC ${PROJECT_NAME} )
target_link_libraries( ${TEST_NAME} PUBLIC sdlpp_application )  # For service registry tests
target_link_libraries( ${TEST_NAME} PUBLIC gtest )
target_link_libraries( ${TEST_NAME} PUBLIC gmock )
target_link_libraries( ${TEST_NAME} PUBLIC gtest_main )

gtest_discover_tests( ${TEST_NAME} 
    DISCOVERY_MODE POST_BUILD
    XML_OUTPUT_DIR report 
    EXTRA_ARGS --gtest_catch_exceptions=0 
    PROPERTIES ENVIRONMENT SDL_VIDEODRIVER=dummy
)

