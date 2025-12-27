# Code coverage integration for SDL++ project
include(FetchContent)

# Function to enable coverage for the project
function(setup_coverage)
    if(NOT SDLXX_ENABLE_COVERAGE)
        return()
    endif()

    # Check for required tools
    # gcov must match the compiler version that generated the .gcda files
    # We detect the compiler version and look for the matching gcov
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # Extract major version from CMAKE_CXX_COMPILER_VERSION (e.g., "13.2.0" -> "13")
        string(REGEX MATCH "^[0-9]+" GCC_MAJOR_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
        find_program(GCOV_PATH NAMES gcov-${GCC_MAJOR_VERSION} gcov)
        if(GCOV_PATH)
            message(STATUS "Using gcov for GCC ${GCC_MAJOR_VERSION}")
        endif()
    else()
        find_program(GCOV_PATH gcov)
    endif()
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

    # Add coverage flags to standard_compiler_options so all project targets get them automatically
    # This is more maintainable than a hardcoded list of targets
    target_compile_options(standard_compiler_options INTERFACE --coverage -fprofile-arcs -ftest-coverage -O0)
    target_link_options(standard_compiler_options INTERFACE --coverage)

    # Create coverage data collection target
    set(COVERAGE_BASE_DIR ${CMAKE_BINARY_DIR}/coverage)
    set(COVERAGE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/coverage)
    
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
    # Ignore gcov errors for files that can't be processed (e.g., from external dependencies)
    list(APPEND GCOVR_COMMAND --gcov-ignore-errors=all)
    list(APPEND GCOVR_COMMAND --gcov-ignore-parse-errors=all)
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
    # Only scan the src build directory, not external dependencies
    list(APPEND GCOVR_COMMAND ${CMAKE_BINARY_DIR}/src)

    add_custom_target(coverage-html
        COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_OUTPUT_DIR}
        COMMAND ${GCOVR_COMMAND}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating HTML coverage report at ${COVERAGE_OUTPUT_DIR}"
        VERBATIM
    )

    # Create convenience target that runs tests and generates coverage
    # Use -C Debug for multi-config generators (Ninja Multi-Config, Visual Studio)
    add_custom_target(coverage
        # Run tests to generate coverage data (ignore test failures, we still want coverage)
        COMMAND ${CMAKE_CTEST_COMMAND} -C Debug --output-on-failure

        # Generate HTML coverage report
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage-html

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating coverage report"
        VERBATIM
    )

endfunction()
