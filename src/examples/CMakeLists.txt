# file(GLOB sources_list LIST_DIRECTORIES true *)
# foreach(dir ${sources_list})
#     if(IS_DIRECTORY ${dir} AND EXISTS "${dir}/CMakeLists.txt")
#         add_subdirectory(${dir})
#     endif()
# endforeach()
include(code_tests)


install_package(NAME SDLXX COMPONENTS SDLXX_Development SDLXX_Runtime)

add_subdirectory(tictactoe)
register_code_test(NAME tictactoe 
    COMPONENTS SDLXX
    USE_FETCHED_DEPENDENCIES SDL3 SDL3_image
)

add_subdirectory(pong)
register_code_test(NAME pong
    COMPONENTS SDLXX
    USE_FETCHED_DEPENDENCIES SDL3 SDL3_image
)
