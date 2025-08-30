# Define output file path
set(FONT_FILE ${CMAKE_CURRENT_BINARY_DIR}/DejaVuSansMono.ttf)

# Create custom command to download and extract font
add_custom_command(
    OUTPUT ${FONT_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Downloading DejaVu Sans Mono font..."
    COMMAND ${CMAKE_COMMAND} 
        -DURL=https://www.fontsquirrel.com/fonts/download/dejavu-sans-mono
        -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/dejavu-sans-mono.zip
        -DEXPECTED_HASH=adada75ccc7cd9627de5b8f5259fdd49ceaba24bb2e58b5726ca9aef17fac47c
        -P ${CMAKE_CURRENT_LIST_DIR}/download_font.cmake
    COMMAND ${CMAKE_COMMAND} 
        -DINPUT=${CMAKE_CURRENT_BINARY_DIR}/dejavu-sans-mono.zip
        -DDESTINATION=${CMAKE_CURRENT_BINARY_DIR}
        -P ${CMAKE_CURRENT_LIST_DIR}/extract_font.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Downloading and extracting DejaVu Sans Mono font"
    VERBATIM
)

# Create target that depends on the font file
add_custom_target(${PROJECT_NAME}_raw_data DEPENDS ${FONT_FILE})

if(WIN32)
    enable_language(RC)
    set(RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")
    add_library(${PROJECT_NAME}_data OBJECT ${RESOURCE_FILE})
    set_target_properties(${PROJECT_NAME}_data PROPERTIES 
        LINKER_LANGUAGE RC
        RC_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
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
