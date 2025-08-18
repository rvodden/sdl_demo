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
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG v1.17.0
                DOWNLOAD_EXTRACT_TIMESTAMP false
                OVERRIDE_FIND_PACKAGE
                SYSTEM
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

### Google Benchmark ###

# option(SDLXX_USE_VENDORED_GBENCH "Download Google Benchmark from GitHub" ON)
# FetchContent_Declare(
#         googlebenchmark
#         GIT_REPOSITORY https://github.com/google/benchmark.git
#         GIT_TAG main # need main for benchmark::benchmark
#         DOWNLOAD_EXTRACT_TIMESTAMP false
#         SYSTEM
# )
# SET(BENCHMARK_DOWNLOAD_DEPENDENCIES on) # TODO: add this as a target property instead of a global
# SET(HAVE_STD_REGEX ON)
# SET(RUN_HAVE_STD_REGEX 1)
# FetchContent_MakeAvailable(googletest googlebenchmark)


option(SDLXX_USE_VENDORED_SDL3 "Download SDL3 from GitHub" ON)
if(SDLXX_USE_VENDORED_SDL3)
        set(SDL_VULKAN OFF)
        ###
        FetchContent_Declare(
                SDL3
                GIT_REPOSITORY https://github.com/libsdl-org/SDL
                GIT_TAG release-3.2.20
                DOWNLOAD_EXTRACT_TIMESTAMP false
                OVERRIDE_FIND_PACKAGE
                SYSTEM
        )
        
        FetchContent_Declare(
                SDL3_image
                GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
                GIT_TAG release-3.2.4
                DOWNLOAD_EXTRACT_TIMESTAMP false
                OVERRIDE_FIND_PACKAGE
                SYSTEM
        )
        set(SDL3IMAGE_INSTALL OFF)
endif()

if(SDLXX_USE_VENDORED_GTEST)
        FetchContent_MakeAvailable(googletest)
endif()

if(SDLXX_USE_VENDORED_SDL3)
        FetchContent_MakeAvailable(SDL3)
        FetchContent_MakeAvailable(SDL3_image)
endif()
