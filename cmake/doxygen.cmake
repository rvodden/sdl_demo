# Doxygen integration for documentation generation
include(FetchContent)

function(setup_doxygen)
    # Find Doxygen with optional dot support for diagrams
    find_package(Doxygen OPTIONAL_COMPONENTS dot)
    
    if(DOXYGEN_FOUND)
        # Set Doxygen configuration options directly in CMake
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/output)
        set(DOXYGEN_PROJECT_NAME "${PROJECT_NAME}")
        set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")
        set(DOXYGEN_PROJECT_BRIEF "${PROJECT_DESCRIPTION}")
        set(DOXYGEN_GENERATE_TREEVIEW YES)
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
        
        # File patterns
        set(DOXYGEN_FILE_PATTERNS *.h *.hpp *.cpp *.md)
        set(DOXYGEN_RECURSIVE YES)
        set(DOXYGEN_MARKDOWN_SUPPORT YES)
        
        # Exclude ADR files from appearing as top-level pages
        set(DOXYGEN_EXCLUDE_PATTERNS */adr/*.md)
        
        # Use doxygen-awesome-css theme if available
        if(SDLXX_USE_VENDORED_DOXYGEN_AWESOME)
            # Make sure doxygen-awesome is available
            FetchContent_MakeAvailable(doxygen_awesome)
            
            set(DOXYGEN_HTML_EXTRA_STYLESHEET 
                ${doxygen_awesome_SOURCE_DIR}/doxygen-awesome.css
                ${doxygen_awesome_SOURCE_DIR}/doxygen-awesome-sidebar-only.css
                ${doxygen_awesome_SOURCE_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css
            )
            set(DOXYGEN_HTML_EXTRA_FILES 
                ${doxygen_awesome_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js
            )
            set(DOXYGEN_HTML_COLORSTYLE LIGHT)
            message(STATUS "Using doxygen-awesome-css theme from ${doxygen_awesome_SOURCE_DIR}")
        endif()
        
        # Modern Doxygen approach - automatically creates 'docs' target
        # Pass input directories directly to the command
        doxygen_add_docs(docs
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_tools/include  
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_application/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_ttf/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/include
            ${CMAKE_CURRENT_SOURCE_DIR}/docs/pages
            ${CMAKE_CURRENT_SOURCE_DIR}/docs/adr
            COMMENT "Generating API documentation with Doxygen"
        )
        
        # If coverage is enabled, add coverage generation to docs target
        if(SDLXX_ENABLE_COVERAGE)
            # Check if coverage targets exist (they're created in coverage.cmake)
            if(TARGET coverage-html)
                add_dependencies(docs coverage-html)
                message(STATUS "Coverage reports will be integrated with documentation")
            endif()
        endif()

        message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")
        message(STATUS "Documentation target 'docs' available - run 'cmake --build --preset <PRESET> --target docs'")
    else()
        message(STATUS "Doxygen not found, documentation target not available")
    endif()
endfunction()
