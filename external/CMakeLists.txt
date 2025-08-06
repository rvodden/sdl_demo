include(FetchContent)

### Google Test ###

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
        DOWNLOAD_EXTRACT_TIMESTAMP false
        SYSTEM
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

### Google Benchmark ###

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


###
FetchContent_Declare(
        SDL2
        URL https://github.com/libsdl-org/SDL/releases/download/release-2.32.8/SDL2-2.32.8.zip
        DOWNLOAD_EXTRACT_TIMESTAMP false
        SYSTEM
)
FetchContent_MakeAvailable(SDL2)

# set(SDL2IMAGE_SAMPLES False)
FetchContent_Declare(
        SDL2_image
        URL https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-2.8.8.zip
        DOWNLOAD_EXTRACT_TIMESTAMP false
        SYSTEM
)
set(SDL2IMAGE_INSTALL OFF)
FetchContent_MakeAvailable(SDL2_image)
