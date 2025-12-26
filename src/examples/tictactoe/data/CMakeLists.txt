embed_resources(
    TARGET ${PROJECT_NAME}
    RESOURCES tic_tac_toe.png
    RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    HEADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include
    NAMESPACE tictactoe
)

target_include_directories(
    ${PROJECT_NAME}-data
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)
