file(GLOB DataFiles data/*)

message(CHECK_START "Proecessing data files...")
foreach (DataFile IN LISTS DataFiles)
    get_filename_component(DataName ${DataFile} NAME)
    set(OutFile "${CMAKE_CURRENT_BINARY_DIR}/${DataName}.o")
    message("   Creating ${OutFile} from ${DataName}")
    add_custom_command(
            OUTPUT ${OutFile}
            MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/data/${DataName}"
            COMMAND "${CMAKE_LINKER}" --relocatable --format binary --output=${OutFile} ${DataName}
            COMMAND objcopy --add-section .note.GNU-stack=/dev/null --set-section-flags .note.GNU-stack=noload ${OutFile}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/${DataName}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
            )
    list(APPEND DataObjectFiles ${OutFile})
endforeach ()
message(CHECK_PASS "done.")

add_library(data OBJECT ${DataFiles})
set_target_properties(data PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories( 
    data    
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set(DataObjectFiles ${DataObjectFiles} PARENT_SCOPE)
