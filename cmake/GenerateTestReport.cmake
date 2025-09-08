# Generate HTML test reports from CTest JUnit XML output
#
# Usage:
#   cmake -P cmake/GenerateTestReport.cmake
#   cmake -DOUTPUT_DIR=docs/test-reports -P cmake/GenerateTestReport.cmake
#   cmake -DXML_FILES="file1.xml;file2.xml" -P cmake/GenerateTestReport.cmake

cmake_minimum_required(VERSION 3.20)

# Default values
if(NOT DEFINED OUTPUT_DIR)
    set(OUTPUT_DIR "test-report")
endif()

if(NOT DEFINED XML_FILES)
    # Auto-discover XML files in common locations
    file(GLOB XML_FILES 
        "${CMAKE_CURRENT_LIST_DIR}/../build/*/test-results-*.xml"
        "${CMAKE_CURRENT_LIST_DIR}/../test-results-*.xml"
        "${CMAKE_CURRENT_LIST_DIR}/../*.xml"
    )
endif()

message(STATUS "Generating test reports in: ${OUTPUT_DIR}")
message(STATUS "Processing XML files: ${XML_FILES}")

# Create output directory
file(MAKE_DIRECTORY "${OUTPUT_DIR}")

# Check if junit2html is available
find_program(JUNIT2HTML_EXECUTABLE junit2html)
if(NOT JUNIT2HTML_EXECUTABLE)
    message(WARNING "junit2html not found. Install with: pip3 install junit2html")
    message(STATUS "Generating basic index.html without individual reports")
    set(JUNIT2HTML_AVAILABLE FALSE)
else()
    set(JUNIT2HTML_AVAILABLE TRUE)
    message(STATUS "Found junit2html: ${JUNIT2HTML_EXECUTABLE}")
endif()

# Generate HTML report for each XML file
set(REPORT_FILES "")
foreach(XML_FILE IN LISTS XML_FILES)
    if(EXISTS "${XML_FILE}")
        get_filename_component(BASE_NAME "${XML_FILE}" NAME_WE)
        set(HTML_FILE "${BASE_NAME}.html")
        
        message(STATUS "Processing ${XML_FILE} -> ${OUTPUT_DIR}/${HTML_FILE}")
        
        if(JUNIT2HTML_AVAILABLE)
            execute_process(
                COMMAND "${JUNIT2HTML_EXECUTABLE}" 
                    "${XML_FILE}"
                    "${OUTPUT_DIR}/${HTML_FILE}"
                RESULT_VARIABLE JUNIT_RESULT
                OUTPUT_VARIABLE JUNIT_OUTPUT
                ERROR_VARIABLE JUNIT_ERROR
            )
            
            if(JUNIT_RESULT EQUAL 0)
                list(APPEND REPORT_FILES "${HTML_FILE}")
                message(STATUS "✓ Generated ${HTML_FILE}")
            else()
                message(WARNING "⚠ Failed to process ${XML_FILE}")
                if(JUNIT_ERROR)
                    message(STATUS "Error output: ${JUNIT_ERROR}")
                endif()
                if(JUNIT_OUTPUT)
                    message(STATUS "Output: ${JUNIT_OUTPUT}")
                endif()
            endif()
        endif()
    else()
        message(WARNING "⚠ File not found: ${XML_FILE}")
    endif()
endforeach()

# Get git information if available
find_program(GIT_EXECUTABLE git)
if(GIT_EXECUTABLE AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/../.git")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.."
        OUTPUT_VARIABLE GIT_COMMIT_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" branch --show-current
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.."
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_COMMIT_SHORT "unknown")
    set(GIT_BRANCH "unknown")
endif()

# Use environment variables if available (for CI)
if(DEFINED ENV{GITHUB_SHA})
    string(SUBSTRING "$ENV{GITHUB_SHA}" 0 8 GIT_COMMIT_SHORT)
endif()
if(DEFINED ENV{GITHUB_REF_NAME})
    set(GIT_BRANCH "$ENV{GITHUB_REF_NAME}")
endif()

# Get current timestamp
string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S UTC" UTC)

# Helper function to generate platform config links
function(generate_config_link PLATFORM CONFIG OUTPUT_VAR)
    # Look for matching report files
    set(FOUND_REPORT "")
    foreach(REPORT_FILE IN LISTS REPORT_FILES)
        string(FIND "${REPORT_FILE}" "test-results-${PLATFORM}" PLATFORM_MATCH)
        string(FIND "${REPORT_FILE}" "-${CONFIG}.html" CONFIG_MATCH)
        if(NOT PLATFORM_MATCH EQUAL -1 AND NOT CONFIG_MATCH EQUAL -1)
            set(FOUND_REPORT "${REPORT_FILE}")
            break()
        endif()
    endforeach()
    
    if(FOUND_REPORT)
        set(${OUTPUT_VAR} "<a href=\"${FOUND_REPORT}\">${CONFIG} Build</a>" PARENT_SCOPE)
    else()
        set(${OUTPUT_VAR} "<span class=\"no-results\">${CONFIG} Build (No Results)</span>" PARENT_SCOPE)
    endif()
endfunction()

# Generate platform-specific links
generate_config_link("ubuntu" "Debug" UBUNTU_DEBUG_LINK)
generate_config_link("ubuntu" "Release" UBUNTU_RELEASE_LINK)
generate_config_link("windows" "Debug" WINDOWS_DEBUG_LINK)
generate_config_link("windows" "Release" WINDOWS_RELEASE_LINK)

# Generate reports section
list(LENGTH REPORT_FILES NUM_REPORTS)
if(NUM_REPORTS GREATER 0)
    set(REPORTS_SECTION "<div class=\"reports-list\">
        <h3>All Test Reports</h3>
        <ul>")
    
    foreach(REPORT_FILE IN LISTS REPORT_FILES)
        get_filename_component(REPORT_NAME "${REPORT_FILE}" NAME_WE)
        string(APPEND REPORTS_SECTION "
            <li><a href=\"${REPORT_FILE}\">${REPORT_NAME}</a></li>")
    endforeach()
    
    string(APPEND REPORTS_SECTION "
        </ul>
    </div>")
else()
    set(REPORTS_SECTION "<div class=\"no-results-section\">
        <h3>No Test Results Found</h3>
        <p>No valid JUnit XML files were processed. To generate test results:</p>
        <ol>
            <li>Configure: <code>cmake --preset ninja-mc</code></li>
            <li>Build: <code>cmake --build --preset ninja-mc-debug</code></li>
            <li>Test: <code>ctest --preset ninja-mc-debug --output-junit test-results.xml</code></li>
            <li>Generate report: <code>cmake -DXML_FILES=\"test-results.xml\" -P cmake/GenerateTestReport.cmake</code></li>
        </ol>
    </div>")
endif()

# Configure the template
message(STATUS "Configuring HTML template...")
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/test-report-template.html.in" 
    "${OUTPUT_DIR}/index.html" 
    @ONLY
)

message(STATUS "✓ Generated index.html from template")
message(STATUS "")
message(STATUS "Test report generated successfully!")

# List generated files
file(GLOB GENERATED_FILES "${OUTPUT_DIR}/*")
foreach(FILE IN LISTS GENERATED_FILES)
    get_filename_component(FILENAME "${FILE}" NAME)
    message(STATUS "  ${FILENAME}")
endforeach()

message(STATUS "")
message(STATUS "View the report by opening: ${OUTPUT_DIR}/index.html")