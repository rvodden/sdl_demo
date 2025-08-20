if(WIN32)
    enable_language(RC)
      set(RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")
      add_library(${PROJECT_NAME}_data STATIC)
      target_sources(${PROJECT_NAME}_data PRIVATE ${RESOURCE_FILE})
else()
    file(GLOB DataFiles *)

    set(SKIP_FILES "CMakeLists.txt" "include")

    foreach (DataFile IN LISTS DataFiles)
        get_filename_component(DataName ${DataFile} NAME)
        if(DataName IN_LIST SKIP_FILES)
            continue()
        endif()
        set(OutFile "${CMAKE_CURRENT_BINARY_DIR}/${DataName}.o")
        add_custom_command(
                OUTPUT ${OutFile}
                MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${DataName}"
                COMMAND "${CMAKE_LINKER}" --relocatable --format binary --output=${OutFile} ${DataName}
                COMMAND objcopy --add-section .note.GNU-stack=/dev/null --set-section-flags .note.GNU-stack=noload ${OutFile}
                DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${DataName}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
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