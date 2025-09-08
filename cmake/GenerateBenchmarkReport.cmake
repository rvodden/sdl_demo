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

# Function to parse JSON benchmark results
function(parse_json_results json_file benchmark_data_var)
    if(NOT EXISTS "${json_file}")
        message(WARNING "File not found: ${json_file}")
        set(${benchmark_data_var} "" PARENT_SCOPE)
        return()
    endif()
    
    file(READ "${json_file}" json_content)
    
    # Extract benchmark entries by finding name and cpu_time pairs
    # First find all name entries
    set(benchmarks "")
    string(REGEX MATCHALL "\"name\"[ \t\n\r]*:[ \t\n\r]*\"([^\"]+)\"" name_matches "${json_content}")
    string(REGEX MATCHALL "\"cpu_time\"[ \t\n\r]*:[ \t\n\r]*([0-9.]+)" cpu_time_matches "${json_content}")
    
    # Convert matches to lists
    set(names "")
    set(cpu_times "")
    
    foreach(match ${name_matches})
        string(REGEX MATCH "\"([^\"]+)\"" _ "${match}")
        if(CMAKE_MATCH_1)
            list(APPEND names "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    
    foreach(match ${cpu_time_matches})
        string(REGEX MATCH "([0-9.]+)" _ "${match}")
        if(CMAKE_MATCH_1)
            list(APPEND cpu_times "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    
    # Combine names and cpu_times (they should be in the same order)
    list(LENGTH names name_count)
    list(LENGTH cpu_times cpu_time_count)
    message(STATUS "Found ${name_count} names and ${cpu_time_count} cpu_times")
    
    if(${name_count} EQUAL ${cpu_time_count} AND ${name_count} GREATER 0)
        math(EXPR max_index "${name_count} - 1")
        foreach(i RANGE ${max_index})
            list(GET names ${i} bench_name)
            list(GET cpu_times ${i} cpu_time)
            list(APPEND benchmarks "${bench_name};${cpu_time}")
        endforeach()
    else()
        message(WARNING "Mismatch between names (${name_count}) and cpu_times (${cpu_time_count}) in ${json_file}")
    endif()
    
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
    foreach(benchmark ${benchmarks})
        string(REPLACE ";" "=" benchmark_entry "${benchmark}")
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
            
            # Format times
            format_time("${sdl3_time}" sdl3_formatted)
            format_time("${sdlpp_time}" sdlpp_formatted)
            
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