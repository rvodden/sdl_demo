include(FetchContent)

# Hide all warnings for 3rdparty code
if (MSVC)
    add_compile_options(/W0)
else()
    add_compile_options(-w)
endif()

option(SDLXX_USE_VENDORED_GTEST "Download GTest from GitHub" ON)
if(SDLXX_USE_VENDORED_GTEST)
        FetchContent_Declare(
                GTest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG v1.17.0
                DOWNLOAD_EXTRACT_TIMESTAMP false
                OVERRIDE_FIND_PACKAGE
                SYSTEM
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

### Google Benchmark ###

option(SDLXX_USE_VENDORED_GBENCH "Download Google Benchmark from GitHub" ON)
if(SDLXX_USE_VENDORED_GBENCH)
SET(BENCHMARK_DOWNLOAD_DEPENDENCIES on) # TODO: add this as a target property instead of a global
SET(HAVE_STD_REGEX ON)
SET(RUN_HAVE_STD_REGEX 1)
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main # need main for benchmark::benchmark
        DOWNLOAD_EXTRACT_TIMESTAMP false
        GIT_SHALLOW ON
        SYSTEM
)
endif()


option(SDLXX_USE_VENDORED_SDL3 "Download SDL3 from GitHub" ON)
option(SDLXX_USE_SYSTEM_SDL3 "Use system SDL3 instead of fetching" OFF)

if(SDLXX_USE_SYSTEM_SDL3)
        # Use system-provided SDL3
        find_package(SDL3 REQUIRED)
        find_package(SDL3_image REQUIRED)
        find_package(SDL3_ttf REQUIRED)
elseif(SDLXX_USE_VENDORED_SDL3)
        # Disable installation of vendored SDL3 to avoid export conflicts
        set(SDL_INSTALL ON)
        set(SDL_VULKAN OFF)
        if(CCACHE_PROGRAM)
                set(SDL_CCACHE ON)
        endif()
        ###
        FetchContent_Declare(
                SDL3
                GIT_REPOSITORY https://github.com/libsdl-org/SDL
                GIT_TAG release-3.2.20
                GIT_SHALLOW ON
                DOWNLOAD_EXTRACT_TIMESTAMP false
                OVERRIDE_FIND_PACKAGE
                SYSTEM
        )
       
        set(SDLIMAGE_AVIF OFF)
        set(SDLIMAGE_DAV1D OFF)
        set(SDLIMAGE_AOM OFF)
        set(SDLIMAGE_TIF OFF)
        set(SDLIMAGE_WEBP OFF)
        set(SDLIMAGE_INSTALL ON)
        FetchContent_Declare(
                SDL3_image
                GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
                GIT_TAG release-3.2.4
                GIT_SHALLOW ON
                DOWNLOAD_EXTRACT_TIMESTAMP false
                OVERRIDE_FIND_PACKAGE
                SYSTEM
        )

        set(SDLTTF_VENDORED ON)
        set(SDLTTF_INSTALL ON)
        FetchContent_Declare(
                SDL3_ttf
                GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
                GIT_TAG release-3.2.2
                DOWNLOAD_EXTRACT_TIMESTAMP false
                OVERRIDE_FIND_PACKAGE
                SYSTEM
        )
endif()

if(SDLXX_USE_VENDORED_GTEST)
        FetchContent_MakeAvailable(GTest)
endif()

if(SDLXX_USE_VENDORED_GBENCH AND SDLXX_ENABLE_BENCHMARKS)
        # Set math library requirement before fetching
        if(UNIX AND NOT APPLE)
            set(BENCHMARK_ENABLE_GTEST_TESTS ON)
            set(BENCHMARK_USE_BUNDLED_GTEST OFF)
        endif()
        FetchContent_MakeAvailable(googlebenchmark)
        # Link math library to benchmark targets on Unix systems
        if(UNIX AND NOT APPLE)
            target_link_libraries(benchmark PUBLIC m)
            if(TARGET benchmark_main)
                target_link_libraries(benchmark_main PUBLIC m)
            endif()
        endif()
endif()

# Doxygen Awesome CSS theme for documentation
option(SDLXX_USE_VENDORED_DOXYGEN_AWESOME "Download Doxygen Awesome CSS from GitHub" ON)
if(SDLXX_USE_VENDORED_DOXYGEN_AWESOME)
    FetchContent_Declare(
        doxygen_awesome
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
        GIT_TAG v2.3.4
        DOWNLOAD_EXTRACT_TIMESTAMP false
        SYSTEM
    )
    # Don't call MakeAvailable here - let the doxygen setup handle it
endif()

if(SDLXX_USE_VENDORED_SDL3 AND NOT SDLXX_USE_SYSTEM_SDL3)
        FetchContent_MakeAvailable(SDL3)
        FetchContent_MakeAvailable(SDL3_image)
        FetchContent_MakeAvailable(SDL3_ttf)
endif()
