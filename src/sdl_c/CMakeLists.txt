add_executable(sdl_c src/main.cxx)
target_link_libraries(sdl_c PRIVATE SDL2::SDL2)
target_link_libraries(sdl_c PRIVATE SDL2_image::SDL2_image)
target_link_libraries(sdl_c PRIVATE data ${DataObjectFiles})
