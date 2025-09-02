# Define output file path
set(FONT_FILE "${CMAKE_CURRENT_BINARY_DIR}/PressStart2P-Regular.ttf")

# Create custom command to download and extract font
add_custom_command(
    OUTPUT ${FONT_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Downloading DejaVu Sans Mono font..."
    COMMAND ${CMAKE_COMMAND} 
        -DURL=https://github.com/google/fonts/raw/e324c91423626034f1e10098081182bb88e340db/ofl/pressstart2p/PressStart2P-Regular.ttf
        -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/PressStart2P-Regular.ttf
        -DEXPECTED_HASH=034c77f1f05ec89421e4a63f0e3a4ca1ecf852cc6d2bf611f126f275728e017d
        -P ${CMAKE_CURRENT_LIST_DIR}/download_font.cmake
    COMMENT "Downloading and extracting DejaVu Sans Mono font"
    VERBATIM
)

# Create target that depends on the font file
add_custom_target(${PROJECT_NAME}_raw_data DEPENDS ${FONT_FILE})

if(WIN32)
    enable_language(RC)
    
    # Configure the resource file with the correct font path
    set(RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/resources.rc")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources.rc.in" ${RESOURCE_FILE} @ONLY)
    
    add_library(${PROJECT_NAME}_data OBJECT ${RESOURCE_FILE})
    set_target_properties(${PROJECT_NAME}_data PROPERTIES 
        LINKER_LANGUAGE RC
        RC_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
    
    # Make sure font is downloaded before configuring resources
    add_dependencies(${PROJECT_NAME}_data ${PROJECT_NAME}_raw_data)
else()
    set(DataFiles ${FONT_FILE})

    foreach (DataFile IN LISTS DataFiles)
        get_filename_component(DataName ${DataFile} NAME)
        if(DataName IN_LIST SKIP_FILES)
            continue()
        endif()
        set(OutFile "${CMAKE_CURRENT_BINARY_DIR}/${DataName}.o")
        add_custom_command(
                OUTPUT ${OutFile}
                MAIN_DEPENDENCY ${DataFile}
                COMMAND "${CMAKE_LINKER}" --relocatable --format binary --output=${OutFile} ${DataName}
                COMMAND objcopy --add-section .note.GNU-stack=/dev/null --set-section-flags .note.GNU-stack=noload ${OutFile}
                DEPENDS ${DataFile}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                )
        list(APPEND DataObjectFiles ${OutFile})
    endforeach ()

    add_library(${PROJECT_NAME}_data STATIC)
    target_sources(${PROJECT_NAME}_data PRIVATE ${DataObjectFiles})
    set_target_properties(${PROJECT_NAME}_data PROPERTIES LINKER_LANGUAGE CXX)

endif()
    
target_include_directories( 
    ${PROJECT_NAME}_data    
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_dependencies(${PROJECT_NAME}_data ${PROJECT_NAME}_raw_data)
