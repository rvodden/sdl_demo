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
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(GTEST_CREATE_SHARED_LIBRARY 0 CACHE BOOL "" FORCE)
        set(GTEST_LINKED_AS_SHARED_LIBRARY 0 CACHE BOOL "" FORCE)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
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
        # set(SDL_INSTALL_CMAKEDIR_ROOT ${CMAKE_INSTALL_LIBDIR}/cmake/SDL3/ CACHE STRING "" FORCE)
        set(SDL_INSTALL ON)
        set(SDL_VULKAN ON)
        set(SDL_SHARED ON)
        set(SDL_STATIC ON)
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
        set(SDLIMAGE_BUILD_SHARED_LIBS ON)
        set(SDLIMAGE_BUILD_STATIC_LIBS ON)
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


        set(SDLSHADERCROSS_DXC OFF CACHE BOOL "Disable DirectXShaderCompiler to avoid compatibility issues" FORCE)
        set(SPIRV_SKIP_TESTS ON CACHE BOOL "Disable SPIRV-Cross tests" FORCE)
        set(SDLSHADERCROSS_VENDORED ON CACHE BOOL "Use vendored dependencies for SDL_shadercross")
        set(SDLSHADERCROSS_SPIRVCROSS_SHARED OFF CACHE BOOL "Link to shared library variants of dependencies")

        # DirectX Shader Compiler binaries for Windows and Linux
        option(SDLXX_USE_VENDORED_DXC "Download DirectX Shader Compiler binaries" OFF)
        if(SDLXX_USE_VENDORED_DXC)
            if(WIN32)
                FetchContent_Declare(
                    dxc_windows
                    URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2505/dxc_2025_05_24.zip
                    URL_HASH SHA256=81380f3eca156d902d6404fd6df9f4b0886f576ff3e18b2cc10d3075ffc9d119
                    DOWNLOAD_EXTRACT_TIMESTAMP false
                    SYSTEM
                )
            else()
                FetchContent_Declare(
                    dxc_linux
                    URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2505/linux_dxc_2025_05_24.x86_64.tar.gz
                    URL_HASH SHA256=b99655f65215287825fcdd49102b17e2a1608eff79ffaf9457514c2676892aa5
                    DOWNLOAD_EXTRACT_TIMESTAMP false
                    SYSTEM
                )
            endif()
        endif()

        FetchContent_Declare(
                SDL_shadercross
                GIT_REPOSITORY https://github.com/libsdl-org/SDL_shadercross
                GIT_TAG main
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
        FetchContent_MakeAvailable(SDL_shadercross)
endif()

if(SDLXX_USE_VENDORED_DXC)
        if(WIN32)
                FetchContent_MakeAvailable(dxc_windows)
        else()
                FetchContent_MakeAvailable(dxc_linux)
        endif()
endif()

### Resource Tools ###
# Disable resource_tools tests - we only need the library
set(RESOURCE_TOOLS_BUILD_TESTS OFF CACHE BOOL "Disable resource_tools tests" FORCE)
set(BUILD_TESTING_SAVED ${BUILD_TESTING})
set(BUILD_TESTING OFF CACHE BOOL "Disable testing for resource_tools" FORCE)

FetchContent_Declare(
        resource_tools
        GIT_REPOSITORY https://github.com/rvodden/resource_tools.git
        GIT_TAG main
        DOWNLOAD_EXTRACT_TIMESTAMP false
        OVERRIDE_FIND_PACKAGE
        SYSTEM
)
FetchContent_MakeAvailable(resource_tools)

# Re-enable testing for our project
set(BUILD_TESTING ${BUILD_TESTING_SAVED} CACHE BOOL "Restore testing state" FORCE)

# Note: resource_tools already has install() commands, so it will be installed
# automatically when SDLXX is installed. The SDLXXConfig.cmake file will find it.
