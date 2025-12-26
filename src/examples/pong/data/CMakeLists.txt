# Define output file path
set(FONT_FILE "${CMAKE_CURRENT_BINARY_DIR}/PressStart2P-Regular.ttf")

# Download font during configuration if it doesn't exist
if(NOT EXISTS ${FONT_FILE})
    message(STATUS "Downloading Press Start 2P font...")
    file(DOWNLOAD
        https://github.com/google/fonts/raw/e324c91423626034f1e10098081182bb88e340db/ofl/pressstart2p/PressStart2P-Regular.ttf
        ${FONT_FILE}
        EXPECTED_HASH SHA256=034c77f1f05ec89421e4a63f0e3a4ca1ecf852cc6d2bf611f126f275728e017d
        SHOW_PROGRESS
        STATUS download_status
    )
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download Press Start 2P font")
    endif()
endif()

# Embed the downloaded font (use relative path from RESOURCE_DIR)
embed_resources(
    TARGET ${PROJECT_NAME}
    RESOURCES PressStart2P-Regular.ttf
    RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}
    HEADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/include
    NAMESPACE pong
)

target_include_directories(
    ${PROJECT_NAME}-data
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)
