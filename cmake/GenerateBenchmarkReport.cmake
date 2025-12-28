# CMake script to generate benchmark HTML report
# Usage: cmake -P GenerateBenchmarkReport.cmake
#        -DBENCHMARK_RESULTS_DIR=/path/to/results
#        -DOUTPUT_DIR=/path/to/output
#        [-DCOMMIT_SHA=abc123]

if(NOT BENCHMARK_RESULTS_DIR)
    message(FATAL_ERROR "BENCHMARK_RESULTS_DIR must be specified")
endif()

if(NOT OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR must be specified")
endif()

if(NOT COMMIT_SHA)
    set(COMMIT_SHA "unknown")
endif()

# Get current date
string(TIMESTAMP CURRENT_DATE "%Y-%m-%d" UTC)

# Function to format time from nanoseconds
function(format_time ns_value output_var)
    if(ns_value LESS 1000)
        math(EXPR formatted_value "${ns_value}")
        set(${output_var} "${formatted_value}ns" PARENT_SCOPE)
    elseif(ns_value LESS 1000000)
        math(EXPR formatted_value "${ns_value} / 1000")
        set(${output_var} "${formatted_value}μs" PARENT_SCOPE)
    elseif(ns_value LESS 1000000000)
        math(EXPR formatted_value "${ns_value} / 1000000")
        set(${output_var} "${formatted_value}ms" PARENT_SCOPE)
    else()
        math(EXPR formatted_value "${ns_value} / 1000000000")
        set(${output_var} "${formatted_value}s" PARENT_SCOPE)
    endif()
endfunction()

# Function to parse JSON benchmark results using CMake's built-in JSON support
function(parse_json_results json_file benchmark_data_var)
    if(NOT EXISTS "${json_file}")
        message(WARNING "File not found: ${json_file}")
        set(${benchmark_data_var} "" PARENT_SCOPE)
        return()
    endif()
    
    file(READ "${json_file}" json_content)
    
    # Parse JSON and extract benchmarks array
    set(benchmarks "")
    
    # Check if the JSON is valid and has a benchmarks array
    string(JSON has_benchmarks ERROR_VARIABLE json_error TYPE "${json_content}" "benchmarks")
    if(json_error OR NOT has_benchmarks STREQUAL "ARRAY")
        message(WARNING "Invalid JSON or missing 'benchmarks' array in ${json_file}: ${json_error}")
        set(${benchmark_data_var} "" PARENT_SCOPE)
        return()
    endif()
    
    # Get the benchmarks array and its length
    string(JSON benchmarks_array GET "${json_content}" "benchmarks")
    string(JSON array_length LENGTH "${benchmarks_array}")
    
    if(array_length EQUAL 0)
        message(STATUS "No benchmarks found in ${json_file}")
        set(${benchmark_data_var} "" PARENT_SCOPE)
        return()
    endif()
    
    # Iterate through each benchmark entry
    math(EXPR max_index "${array_length} - 1")
    foreach(i RANGE ${max_index})
        # Extract name first to check aggregate type
        string(JSON bench_name ERROR_VARIABLE name_error GET "${benchmarks_array}" ${i} "name")
        if(name_error)
            continue()
        endif()

        # Check if this is an aggregate result
        string(JSON run_type ERROR_VARIABLE run_type_error GET "${benchmarks_array}" ${i} "run_type")
        if(NOT run_type_error AND run_type STREQUAL "aggregate")
            # For aggregate results, only use the "mean" values (skip median, stddev, cv)
            string(JSON aggregate_name ERROR_VARIABLE agg_error GET "${benchmarks_array}" ${i} "aggregate_name")
            if(agg_error OR NOT aggregate_name STREQUAL "mean")
                continue()
            endif()
            # Remove the _mean suffix from the name for processing
            string(REGEX REPLACE "_mean$" "" bench_name "${bench_name}")
        endif()

        # Extract cpu_time for each benchmark
        string(JSON cpu_time ERROR_VARIABLE time_error GET "${benchmarks_array}" ${i} "cpu_time")

        if(time_error)
            message(STATUS "Skipping benchmark ${i} (missing cpu_time): ${bench_name}")
            continue()
        endif()
        
        # Convert floating point cpu_time to integer for math operations
        string(REGEX REPLACE "\\.[0-9]*" "" cpu_time_int "${cpu_time}")
        list(APPEND benchmarks "${bench_name}=${cpu_time_int}")
    endforeach()
    
    list(LENGTH benchmarks benchmark_count)
    message(STATUS "Parsed ${benchmark_count} benchmarks from ${json_file}")
    set(${benchmark_data_var} "${benchmarks}" PARENT_SCOPE)
endfunction()

# Function to generate platform section HTML using templates
function(generate_platform_section benchmarks platform_name platform_icon output_var)
    set(TEMPLATE_DIR "${CMAKE_CURRENT_LIST_DIR}/templates")
    
    if(NOT benchmarks)
        # Use no-data template
        set(PLATFORM_NAME "${platform_name}")
        set(PLATFORM_ICON "${platform_icon}")
        configure_file("${TEMPLATE_DIR}/no-data-section.html.in" 
                       "${CMAKE_CURRENT_BINARY_DIR}/temp_no_data.html" 
                       @ONLY)
        file(READ "${CMAKE_CURRENT_BINARY_DIR}/temp_no_data.html" section_html)
        set(${output_var} "${section_html}" PARENT_SCOPE)
        return()
    endif()
    
    # Group benchmarks by pairs
    set(benchmark_pairs "")
    foreach(benchmark_entry ${benchmarks})
        string(REGEX MATCH "([^=]+)=([0-9.]+)" _ "${benchmark_entry}")
        set(bench_name "${CMAKE_MATCH_1}")
        set(cpu_time "${CMAKE_MATCH_2}")
        
        # Determine if this is SDL++ or SDL3
        string(FIND "${bench_name}" "SDLpp" is_sdlpp_pos)
        string(REGEX REPLACE "_(SDL3|SDLpp)" "" base_name "${bench_name}")
        string(REGEX REPLACE ".*/(.*)$" "\\1" base_name "${base_name}")
        
        if(is_sdlpp_pos GREATER -1)
            set(sdlpp_${base_name} "${cpu_time}")
        else()
            set(sdl3_${base_name} "${cpu_time}")
        endif()
        
        list(APPEND benchmark_names "${base_name}")
    endforeach()
    
    list(REMOVE_DUPLICATES benchmark_names)
    list(SORT benchmark_names)
    
    set(table_rows "")
    foreach(base_name ${benchmark_names})
        if(DEFINED sdl3_${base_name} AND DEFINED sdlpp_${base_name})
            set(sdl3_time "${sdl3_${base_name}}")
            set(sdlpp_time "${sdlpp_${base_name}}")
            
            # Calculate overhead percentage
            math(EXPR overhead_calc "(${sdlpp_time} - ${sdl3_time}) * 100 / ${sdl3_time}")
            
            # Determine overhead class
            if(overhead_calc GREATER 20)
                set(overhead_class "overhead-bad")
            elseif(overhead_calc GREATER 10)
                set(overhead_class "overhead-ok")
            else()
                set(overhead_class "overhead-good")
            endif()
            
            # Format times (convert back to original floating point for display)
            set(sdl3_formatted "${sdl3_time}ns")
            set(sdlpp_formatted "${sdlpp_time}ns")
            
            # Format overhead
            if(overhead_calc GREATER 0)
                set(overhead_formatted "+${overhead_calc}%")
            else()
                set(overhead_formatted "${overhead_calc}%")
            endif()
            
            set(table_rows "${table_rows}
                    <tr>
                        <td class=\"benchmark-name\">${base_name}</td>
                        <td class=\"sdl3-time\">${sdl3_formatted}</td>
                        <td class=\"sdlpp-time\">${sdlpp_formatted}</td>
                        <td><span class=\"overhead ${overhead_class}\">${overhead_formatted}</span></td>
                    </tr>")
        endif()
    endforeach()
    
    # Use platform section template
    set(PLATFORM_NAME "${platform_name}")
    set(PLATFORM_ICON "${platform_icon}")
    set(TABLE_ROWS "${table_rows}")
    configure_file("${TEMPLATE_DIR}/platform-section.html.in" 
                   "${CMAKE_CURRENT_BINARY_DIR}/temp_platform.html" 
                   @ONLY)
    file(READ "${CMAKE_CURRENT_BINARY_DIR}/temp_platform.html" section_html)
    set(${output_var} "${section_html}" PARENT_SCOPE)
endfunction()

# Parse benchmark results
parse_json_results("${BENCHMARK_RESULTS_DIR}/ubuntu-results.json" ubuntu_benchmarks)
parse_json_results("${BENCHMARK_RESULTS_DIR}/windows-results.json" windows_benchmarks)

# Generate platform sections
generate_platform_section("${ubuntu_benchmarks}" "Ubuntu Linux" "🐧" ubuntu_section)
generate_platform_section("${windows_benchmarks}" "Windows" "🪟" windows_section)

# Combine platform sections
set(PLATFORM_SECTIONS "${ubuntu_section}${windows_section}")

# Use main template to generate final HTML
set(TEMPLATE_DIR "${CMAKE_CURRENT_LIST_DIR}/templates")
configure_file("${TEMPLATE_DIR}/benchmark-report.html.in" 
               "${OUTPUT_DIR}/index.html" 
               @ONLY)

message(STATUS "Benchmark report generated successfully at ${OUTPUT_DIR}/index.html")