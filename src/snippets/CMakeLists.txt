file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
# Remove application files from the list since they need special handling

# Add shaders subdirectory for GLSL compilation
add_subdirectory(shaders)

# SDL_shadercross compatibility issues have been resolved

foreach(SOURCE_FILE ${SOURCE_FILES})
    get_filename_component(ExecutableName ${SOURCE_FILE} NAME_WLE)
    add_executable(${ExecutableName} ${SOURCE_FILE})
    target_include_directories(${ExecutableName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${ExecutableName} PRIVATE standard_compiler_options)
endforeach()
    
target_link_libraries(font_hello_world PRIVATE sdl_ttf)

# application files need special handling - only link to sdl_main which provides everything
target_link_libraries(application_example PRIVATE sdl::sdl_main)
target_link_libraries(custom_events_example PRIVATE sdl::sdl sdl::sdl_tools)
target_link_libraries(font_hello_world PRIVATE sdl::sdl_main)
target_link_libraries(event_adaptor_example PRIVATE SDL3::SDL3 sdl::sdl sdl::sdl_tools sdl::sdl-private)
target_link_libraries(keyboard_event_example PRIVATE sdl::sdl_main sdl::sdl_ttf)
target_link_libraries(lambda_event_handler_test PRIVATE sdl::sdl sdl::sdl_tools)
target_link_libraries(sdl_driver_test PRIVATE SDL3::SDL3 sdl::sdl sdl::sdl_tools)
target_link_libraries(test_sdl_app_event PRIVATE sdl::sdl_main)
target_link_libraries(message_box_demo PRIVATE sdl::sdl-static)

# SDL_shadercross demonstrations
target_link_libraries(shader_demo_success PRIVATE SDL3::SDL3 SDL3_shadercross::SDL3_shadercross shader_headers)
target_link_libraries(shader_demo_working PRIVATE SDL3::SDL3 SDL3_shadercross::SDL3_shadercross shader_headers)

# Basic SDL3 GPU test
target_link_libraries(simple_test PRIVATE SDL3::SDL3)
