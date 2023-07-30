add_executable(sdl_oop src/main.cxx)
add_dependency(sdl_oop PUBLIC sdl)
add_dependency(sdl_oop PUBLIC sdl_tools)
target_link_libraries(sdl_oop PRIVATE data ${DataObjectFiles})
