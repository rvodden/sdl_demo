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
    USE_FETCHED_DEPENDENCIES SDL3 SDL3_image SDL3_ttf
)
