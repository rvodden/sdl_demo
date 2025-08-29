project(sdl_application LANGUAGES CXX VERSION ${PROJECT_VERSION})

add_library(${PROJECT_NAME} SHARED)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

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
            FILES ${CMAKE_CURRENT_BINARY_DIR}/sdl_application_export.h
    )

target_link_libraries(${PROJECT_NAME} PUBLIC sdl)
target_link_libraries(${PROJECT_NAME} PUBLIC sdl_tools)
target_link_libraries(${PROJECT_NAME} PRIVATE sdl_private)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

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
target_link_libraries( ${TEST_NAME} PUBLIC gtest )
target_link_libraries( ${TEST_NAME} PUBLIC gmock )
target_link_libraries( ${TEST_NAME} PUBLIC gtest_main )

gtest_discover_tests( ${TEST_NAME} 
    DISCOVERY_MODE POST_BUILD
    XML_OUTPUT_DIR report 
    EXTRA_ARGS --gtest_catch_exceptions=0 
    PROPERTIES ENVIRONMENT SDL_VIDEODRIVER=dummy
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} EXPORT ${TARGET_EXPORT_NAME}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    FILE_SET generated_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${RUNTIME_COMPONENT_NAME} 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${RUNTIME_COMPONENT_NAME} NAMELINK_COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${DEVELOPMENT_COMPONENT_NAME}
)
