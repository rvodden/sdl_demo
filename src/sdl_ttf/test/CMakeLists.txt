set( TEST_NAME "${PROJECT_NAME}_test" )

file( GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h )
add_executable( ${TEST_NAME} ${TEST_SOURCE_FILES} )

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${TEST_NAME} PRIVATE standard_compiler_options)

    if(SDLCPP_USE_PRECOMPILED_HEADERS)
        target_link_libraries(${TEST_NAME} PRIVATE precompiled_headers)
    endif()
endif()

# Disable clang-tidy for test files
set_target_properties( ${TEST_NAME} PROPERTIES CXX_CLANG_TIDY "" )

target_link_libraries( ${TEST_NAME} PUBLIC ${PROJECT_NAME}-static )
target_link_libraries( ${TEST_NAME} PUBLIC sdl::sdl_tools-static )
target_link_libraries( ${TEST_NAME} PUBLIC GTest::gtest )
target_link_libraries( ${TEST_NAME} PUBLIC gmock )
target_link_libraries( ${TEST_NAME} PUBLIC GTest::gtest_main )

gtest_discover_tests( ${TEST_NAME} 
    DISCOVERY_MODE POST_BUILD
    XML_OUTPUT_DIR report 
    EXTRA_ARGS --gtest_catch_exceptions=0 
    PROPERTIES ENVIRONMENT SDL_VIDEODRIVER=dummy
)
