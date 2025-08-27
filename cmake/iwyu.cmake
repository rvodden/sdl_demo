#ifndef CMAKE_IWYU_CMAKE
#define CMAKE_IWYU_CMAKE

# Include What You Use (IWYU) integration
# Provides functions to enable IWYU checking for targets

# Find include-what-you-use tool
find_program(IWYU_TOOL include-what-you-use)

if(IWYU_TOOL)
    message(STATUS "Found include-what-you-use: ${IWYU_TOOL}")
    
    
    # Function to enable IWYU for a specific target
    function(enable_iwyu_for_target target_name)
        if(IWYU_TOOL)
            # Check if target is imported/system target
            get_target_property(target_type ${target_name} TYPE)
            get_target_property(is_imported ${target_name} IMPORTED)
            
            if(is_imported OR target_type STREQUAL "INTERFACE_LIBRARY")
                message(STATUS "Skipping IWYU for system/imported target: ${target_name}")
                return()
            endif()
            
            # Set IWYU as the C++ include checker
            set_property(TARGET ${target_name} 
                PROPERTY CXX_INCLUDE_WHAT_YOU_USE 
                "${IWYU_TOOL};--std=c++20;--mapping_file=${CMAKE_SOURCE_DIR}/cmake/iwyu.imp"
            )
            message(STATUS "IWYU enabled for target: ${target_name}")
        else()
            message(WARNING "IWYU requested for ${target_name} but include-what-you-use not found")
        endif()
    endfunction()
    
    # Function to enable IWYU for all project targets (excludes system/imported targets)
    function(enable_iwyu_for_project_targets)
        if(NOT IWYU_TOOL)
            message(WARNING "IWYU requested but include-what-you-use not found")
            return()
        endif()
        
        # Get all targets in the current directory and subdirectories
        get_property(all_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
        
        foreach(target ${all_targets})
            # Check if target exists and is not imported
            if(TARGET ${target})
                get_target_property(is_imported ${target} IMPORTED)
                get_target_property(target_type ${target} TYPE)
                
                # Skip system/imported targets and interface libraries
                if(NOT is_imported AND NOT target_type STREQUAL "INTERFACE_LIBRARY")
                    enable_iwyu_for_target(${target})
                endif()
            endif()
        endforeach()
    endfunction()
    
    # Enable IWYU for project targets if option is set
    if(SDLXX_USE_IWYU)
        enable_iwyu_for_project_targets()
    endif()
    
else()
    message(STATUS "include-what-you-use not found, IWYU checking disabled")
    
    # Provide stub functions when IWYU is not available
    function(enable_iwyu_for_target target_name)
        message(STATUS "IWYU not available, skipping for target: ${target_name}")
    endfunction()
    
    function(enable_iwyu_for_project_targets)
        message(STATUS "IWYU not available, skipping project targets")
    endfunction()
endif()

#endif // CMAKE_IWYU_CMAKE