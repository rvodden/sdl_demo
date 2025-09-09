include(code_tests)
install_package(NAME SDLXX COMPONENTS SDLXX_Development SDLXX_Runtime)

install_fetched_dependency(NAME SDL3)
install_fetched_dependency(NAME SDL3_image)
install_fetched_dependency(NAME SDL3_ttf)
install_fetched_dependency(NAME GTest COMPONENTS gtest gmock)

add_subdirectory(tictactoe)
register_code_test(NAME tictactoe 
    COMPONENTS SDLXX
    USE_FETCHED_DEPENDENCIES SDL3 SDL3_image SDL3_ttf
)

add_subdirectory(snake)

add_subdirectory(pong)
register_code_test(NAME pong
    COMPONENTS SDLXX
    USE_FETCHED_DEPENDENCIES SDL3 SDL3_image SDL3_ttf GTest
)
