# SDL Event System Benchmarks
# Only built when SDLXX_ENABLE_BENCHMARKS=ON

if(NOT SDLXX_ENABLE_BENCHMARKS)
    return()
endif()

if(NOT TARGET benchmark::benchmark)
    message(WARNING "Google Benchmark not available, skipping SDL benchmarks")
    return()
endif()

# Event system benchmarks
add_executable(sdl_event_benchmarks
    event_system_benchmark.cpp
)

target_link_libraries(sdl_event_benchmarks
    PRIVATE
        sdl
        benchmark::benchmark
        benchmark::benchmark_main
)

target_include_directories(sdl_event_benchmarks
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)

# Set target properties
set_target_properties(sdl_event_benchmarks PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/benchmarks
)

# Enable all compiler optimizations for benchmarks
target_compile_options(sdl_event_benchmarks PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O3 -g -DNDEBUG>
)

# Register benchmark with CTest (optional)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_test(
        NAME sdl_event_benchmarks
        COMMAND sdl_event_benchmarks --benchmark_min_time=0.1s
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    set_tests_properties(sdl_event_benchmarks PROPERTIES
        LABELS "benchmark;performance"
    )
endif()