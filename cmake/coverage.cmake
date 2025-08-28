# Code coverage integration for SDL++ project
include(FetchContent)

# Function to enable coverage for the project
function(setup_coverage)
    if(NOT SDLXX_ENABLE_COVERAGE)
        return()
    endif()

    # Check for required tools
    find_program(GCOV_PATH gcov NAMES gcov-14 gcov)
    find_program(GCOVR_PATH gcovr)
    
    if(NOT GCOV_PATH)
        message(WARNING "gcov not found! Coverage analysis will not be available.")
        return()
    endif()
    
    if(NOT GCOVR_PATH)
        message(WARNING "gcovr not found! Please install gcovr for HTML coverage reports.")
        message(WARNING "On Ubuntu/Debian: sudo apt install gcovr")
        return()
    endif()

    message(STATUS "Code coverage analysis enabled")
    message(STATUS "gcov found: ${GCOV_PATH}")
    message(STATUS "gcovr found: ${GCOVR_PATH}")

    # Coverage compiler flags
    set(COVERAGE_COMPILER_FLAGS "--coverage -fprofile-arcs -ftest-coverage -O0")
    set(COVERAGE_LINKER_FLAGS "--coverage")

    # Store coverage flags for later application
    set(COVERAGE_COMPILER_FLAGS_LIST --coverage -fprofile-arcs -ftest-coverage -O0)
    set(COVERAGE_LINKER_FLAGS_LIST --coverage)
    
    # Store in global properties so they can be used later
    set_property(GLOBAL PROPERTY COVERAGE_COMPILER_FLAGS "${COVERAGE_COMPILER_FLAGS_LIST}")
    set_property(GLOBAL PROPERTY COVERAGE_LINKER_FLAGS "${COVERAGE_LINKER_FLAGS_LIST}")

    # Create coverage data collection target
    set(COVERAGE_BASE_DIR ${CMAKE_BINARY_DIR}/coverage)
    set(COVERAGE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/output/html/coverage)
    
    add_custom_target(coverage-clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${COVERAGE_BASE_DIR}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${COVERAGE_OUTPUT_DIR}
        COMMENT "Cleaning coverage data"
        VERBATIM
    )

    set(GCOVR_COMMAND ${GCOVR_PATH})
    list(APPEND GCOVR_COMMAND -v)
    list(APPEND GCOVR_COMMAND --gcov-executable)
    list(APPEND GCOVR_COMMAND ${GCOV_PATH})
    list(APPEND GCOVR_COMMAND --html-details)
    list(APPEND GCOVR_COMMAND --output)
    list(APPEND GCOVR_COMMAND ${COVERAGE_OUTPUT_DIR}/index.html)
    list(APPEND GCOVR_COMMAND --exclude)
    list(APPEND GCOVR_COMMAND [[.*_test\.cpp$]])
    list(APPEND GCOVR_COMMAND --exclude)
    list(APPEND GCOVR_COMMAND [[.*/visitor_pattern_mockup/.*]])
    list(APPEND GCOVR_COMMAND --exclude)
    list(APPEND GCOVR_COMMAND [[.*/snippets/.*]])
    list(APPEND GCOVR_COMMAND --exclude)
    list(APPEND GCOVR_COMMAND [[.*/examples/.*]])
    list(APPEND GCOVR_COMMAND --root)
    list(APPEND GCOVR_COMMAND ${CMAKE_SOURCE_DIR}/src)
    list(APPEND GCOVR_COMMAND ${CMAKE_BINARY_DIR})

    add_custom_target(coverage-html
        COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_OUTPUT_DIR}
        COMMAND ${GCOVR_COMMAND}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating HTML coverage report at ${COVERAGE_OUTPUT_DIR}"
        VERBATIM
    )

    # Create convenience target that runs tests and generates coverage
    add_custom_target(coverage
        # Run tests to generate coverage data (ignore failures from missing tests)
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure || true
        
        # Generate HTML coverage report  
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage-html
        
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating coverage report"
        VERBATIM
    )

    # Export function for use in main CMakeLists.txt
    set_property(GLOBAL PROPERTY COVERAGE_FUNCTION_AVAILABLE TRUE)
    
endfunction()

# Function to be called after all project targets are defined
function(enable_coverage_for_project_targets)
    if(NOT SDLXX_ENABLE_COVERAGE)
        return()
    endif()
    
    # Get coverage flags from global properties
    get_property(coverage_compile_flags GLOBAL PROPERTY COVERAGE_COMPILER_FLAGS)
    get_property(coverage_link_flags GLOBAL PROPERTY COVERAGE_LINKER_FLAGS)
    
    if(NOT coverage_compile_flags)
        message(WARNING "Coverage flags not found - setup_coverage() may not have been called")
        return()
    endif()
    
    # List of known project target names (not external dependencies)
    set(project_targets
        # Core libraries
        sdl
        sdl_tools  
        sdl_ttf
        sdl_application
        sdl_main
        
        # Applications
        tictactoe
        visitor_pattern_mockup
        
        # Test targets
        sdl_test
        sdl_tools_test
        sdl_ttf_test
        sdl_application_test
        
        # Example/snippet targets that may exist
        custom_events_example
        lambda_event_handler_test
        font_hello_world
    )
    
    foreach(target ${project_targets})
        if(TARGET ${target})
            target_compile_options(${target} PRIVATE ${coverage_compile_flags})
            target_link_options(${target} PRIVATE ${coverage_link_flags})
            message(STATUS "Coverage enabled for target: ${target}")
        endif()
    endforeach()
endfunction()
