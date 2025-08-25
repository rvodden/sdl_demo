file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
# Remove application files from the list since they need special handling

foreach(SOURCE_FILE ${SOURCE_FILES})
    get_filename_component(ExecutableName ${SOURCE_FILE} NAME_WLE)
    add_executable(${ExecutableName} ${SOURCE_FILE})
    target_link_libraries(${ExecutableName} PRIVATE standard_compiler_options)
endforeach()
    
target_link_libraries(font_hello_world PRIVATE sdlpp_ttf)

# application files need special handling - only link to sdlpp_main which provides everything
target_link_libraries(application_example PRIVATE sdlpp_main)
target_link_libraries(custom_events_example PRIVATE sdlpp sdlpp_tools)
target_link_libraries(font_hello_world PRIVATE sdlpp_main)
target_link_libraries(event_adaptor_example PRIVATE SDL3::SDL3 sdlpp sdlpp_tools sdlpp_private utils)
target_link_libraries(lambda_event_handler_test PRIVATE sdlpp sdlpp_tools)
target_link_libraries(sdl_driver_test PRIVATE SDL3::SDL3 sdlpp sdlpp_tools)
target_link_libraries(test_sdl_app_event PRIVATE sdlpp_main)
