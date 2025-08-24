file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
foreach(SOURCE_FILE ${SOURCE_FILES})
    get_filename_component(ExecutableName ${SOURCE_FILE} NAME_WLE)
    add_executable(${ExecutableName} ${SOURCE_FILE})
    target_link_libraries(${ExecutableName} PRIVATE standard_compiler_options SDL3::SDL3 sdlpp sdlpp_tools)
endforeach()
    
target_link_libraries(font_hello_world PRIVATE sdlpp_ttf)
target_link_libraries(application_example PRIVATE sdlpp_main)
