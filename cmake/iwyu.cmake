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
                "${IWYU_TOOL};-Xiwyu;--mapping_file=${CMAKE_SOURCE_DIR}/cmake/iwyu.imp;--std=c++20;-Xiwyu;--no_fwd_decls"
            )
            message(STATUS "IWYU enabled for target: ${target_name}")
        else()
            message(WARNING "IWYU requested for ${target_name} but include-what-you-use not found")
        endif()
    endfunction()
    
    # Function to enable IWYU for all project targets from specific known project modules
    function(enable_iwyu_for_project_targets)
        if(NOT IWYU_TOOL)
            message(WARNING "IWYU requested but include-what-you-use not found")
            return()
        endif()
        
        # List of known project target names (not external dependencies)
        set(project_targets
            utils
            sdl=static sdl_private
            sdl_tools-static sdl_tools_private  
            sdl_ttf-static sdl_ttf_private
            sdl_main-static
            tictactoe tictactoe_data
            snippets
            visitor_pattern_mockup visitor_pattern_mockup_private
            # Test targets
            utils_test
            sdl_test
            sdl_tools_test
            sdl_ttf_test
            sdl_application_test
        )
        
        foreach(target ${project_targets})
            if(TARGET ${target})
                enable_iwyu_for_target(${target})
            endif()
        endforeach()
    endfunction()
    
    # Note: IWYU enablement moved to main CMakeLists.txt after subdirectories are processed
    
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
