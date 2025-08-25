file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
# Remove application files from the list since they need special handling
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*application_example\\.cpp$")
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*test_event_routing\\.cpp$")
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*test_sdl_app_event\\.cpp$")

foreach(SOURCE_FILE ${SOURCE_FILES})
    get_filename_component(ExecutableName ${SOURCE_FILE} NAME_WLE)
    add_executable(${ExecutableName} ${SOURCE_FILE})
    target_link_libraries(${ExecutableName} PRIVATE standard_compiler_options SDL3::SDL3 sdlpp sdlpp_tools sdlpp_private utils)
endforeach()
    
target_link_libraries(font_hello_world PRIVATE sdlpp_ttf)

# application files need special handling - only link to sdlpp_main which provides everything
add_executable(application_example ${CMAKE_CURRENT_SOURCE_DIR}/src/application_example.cpp)
target_link_libraries(application_example PRIVATE standard_compiler_options sdlpp_main)

add_executable(test_event_routing ${CMAKE_CURRENT_SOURCE_DIR}/src/test_event_routing.cpp)
target_link_libraries(test_event_routing PRIVATE standard_compiler_options sdlpp_main)

add_executable(test_sdl_app_event ${CMAKE_CURRENT_SOURCE_DIR}/src/test_sdl_app_event.cpp)
target_link_libraries(test_sdl_app_event PRIVATE standard_compiler_options sdlpp_main)
