project(sdl_main LANGUAGES CXX)

add_library(${PROJECT_NAME} SHARED)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_sources(${PROJECT_NAME} 
    PUBLIC 
        FILE_SET generated_headers
            TYPE HEADERS
            BASE_DIRS $<TARGET_PROPERTY:${PROJECT_NAME},BINARY_DIR>
            FILES ${CMAKE_CURRENT_BINARY_DIR}/sdl_main_export.h
    )

target_link_libraries(${PROJECT_NAME} PUBLIC sdl_application)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME} PRIVATE standard_compiler_options)
    target_link_libraries(${PROJECT_NAME} PRIVATE precompiled_headers)
endif()

if(SDLCPP_USE_CLANG_TIDY)
  set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()
