add_executable(test_project src/main.cxx)
target_link_libraries(test_project PUBLIC SDL2::SDL2)
target_link_libraries(test_project PUBLIC sdl)
