# CMake script to verify generated benchmark HTML report
# Usage: cmake -DHTML_FILE=/path/to/index.html -P VerifyBenchmarkReport.cmake

if(NOT HTML_FILE)
    message(FATAL_ERROR "HTML_FILE must be specified")
endif()

if(NOT EXISTS "${HTML_FILE}")
    message(FATAL_ERROR "Generated HTML file not found: ${HTML_FILE}")
endif()

# Read the generated HTML file
file(READ "${HTML_FILE}" html_content)

# Verify basic HTML structure
if(NOT html_content MATCHES "<!DOCTYPE html>")
    message(FATAL_ERROR "HTML file missing DOCTYPE declaration")
endif()

if(NOT html_content MATCHES "<title>SDL\\+\\+ Performance Benchmarks</title>")
    message(FATAL_ERROR "HTML file missing expected title")
endif()

# Verify platform sections are present
if(NOT html_content MATCHES "Ubuntu Linux")
    message(FATAL_ERROR "HTML file missing Ubuntu Linux section")
endif()

if(NOT html_content MATCHES "Windows")
    message(FATAL_ERROR "HTML file missing Windows section")
endif()

# Verify benchmark data is present
if(NOT html_content MATCHES "BM_EventCreation")
    message(FATAL_ERROR "HTML file missing EventCreation benchmark data")
endif()

if(NOT html_content MATCHES "BM_EventDispatch")
    message(FATAL_ERROR "HTML file missing EventDispatch benchmark data")
endif()

if(NOT html_content MATCHES "BM_WindowCreation")
    message(FATAL_ERROR "HTML file missing WindowCreation benchmark data")
endif()

# Verify SDL3 and SDL++ comparisons
if(NOT html_content MATCHES "sdl3-time")
    message(FATAL_ERROR "HTML file missing SDL3 timing data")
endif()

if(NOT html_content MATCHES "sdlpp-time")
    message(FATAL_ERROR "HTML file missing SDL++ timing data")
endif()

# Verify overhead calculations are present
if(NOT html_content MATCHES "overhead")
    message(FATAL_ERROR "HTML file missing overhead calculations")
endif()

# Verify commit SHA is included (if provided)
if(NOT html_content MATCHES "test123")
    message(FATAL_ERROR "HTML file missing commit SHA")
endif()

# Check file size is reasonable (should be several KB for a proper report)
file(SIZE "${HTML_FILE}" file_size)
if(file_size LESS 2000)
    message(FATAL_ERROR "Generated HTML file too small (${file_size} bytes), likely incomplete")
endif()

message(STATUS "✅ Benchmark report verification passed")
message(STATUS "   - HTML structure: OK")
message(STATUS "   - Platform sections: OK") 
message(STATUS "   - Benchmark data: OK")
message(STATUS "   - SDL3/SDL++ comparisons: OK")
message(STATUS "   - File size: ${file_size} bytes")