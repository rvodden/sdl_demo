# CMake utilities and scripts testing
# This CMakeLists.txt provides tests for CMake scripts in the parent directory

cmake_minimum_required(VERSION 3.19)

# Test for GenerateBenchmarkReport.cmake script
# This test exercises the JSON parsing and HTML report generation

# Create test data and output directories
set(CMAKE_TEST_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/test_data")
set(CMAKE_TEST_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/test_output")

# Create directories at configure time
file(MAKE_DIRECTORY "${CMAKE_TEST_DATA_DIR}")
file(MAKE_DIRECTORY "${CMAKE_TEST_OUTPUT_DIR}")

# Generate test JSON files with realistic SDL++ vs SDL3 benchmark data
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/data/ubuntu-results.json.in"
    "${CMAKE_TEST_DATA_DIR}/ubuntu-results.json"
    @ONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/data/windows-results.json.in"
    "${CMAKE_TEST_DATA_DIR}/windows-results.json"
    @ONLY
)

# Add test that runs the benchmark report generation script
add_test(
    NAME benchmark_report_generation_test
    COMMAND ${CMAKE_COMMAND}
        -DBENCHMARK_RESULTS_DIR=${CMAKE_TEST_DATA_DIR}
        -DOUTPUT_DIR=${CMAKE_TEST_OUTPUT_DIR}
        -DCOMMIT_SHA=test123
        -P ${CMAKE_CURRENT_SOURCE_DIR}/../GenerateBenchmarkReport.cmake
)

# Set test properties
set_tests_properties(benchmark_report_generation_test PROPERTIES
    LABELS "cmake_scripts"
    TIMEOUT 30
)

# Add test to verify the generated HTML file exists and contains expected content
add_test(
    NAME benchmark_report_content_test
    COMMAND ${CMAKE_COMMAND}
        -DHTML_FILE=${CMAKE_TEST_OUTPUT_DIR}/index.html
        -P ${CMAKE_CURRENT_SOURCE_DIR}/VerifyBenchmarkReport.cmake
)

# Make content test depend on generation test
set_tests_properties(benchmark_report_content_test PROPERTIES
    DEPENDS benchmark_report_generation_test
    LABELS "cmake_scripts"
    TIMEOUT 10
)

# Clean up test outputs after testing (optional)
set_property(TEST benchmark_report_generation_test APPEND PROPERTY
    FIXTURES_SETUP benchmark_report_fixture
)

set_property(TEST benchmark_report_content_test APPEND PROPERTY
    FIXTURES_REQUIRED benchmark_report_fixture
)