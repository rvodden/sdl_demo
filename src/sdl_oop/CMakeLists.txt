add_executable(sdl_oop src/main.cxx)
target_link_libraries(sdl_oop PUBLIC sdl)
target_link_libraries(sdl_oop PUBLIC sdl_tools)
target_link_libraries(sdl_oop PRIVATE data ${DataObjectFiles})
