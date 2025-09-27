# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 4.1.0)

# Reject any attempt to use a toolchain file. We must not use one because
# we could be downloading it here. If the CMAKE_TOOLCHAIN_FILE environment
# variable is set, the cache variable will have been initialized from it.
unset(CMAKE_TOOLCHAIN_FILE CACHE)
unset(ENV{CMAKE_TOOLCHAIN_FILE})

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(dxc_linux-populate NONE)



include(ExternalProject)
ExternalProject_Add(dxc_linux-populate
                     "URL" "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2505/linux_dxc_2025_05_24.x86_64.tar.gz" "URL_HASH" "SHA256=b99655f65215287825fcdd49102b17e2a1608eff79ffaf9457514c2676892aa5"
                    SOURCE_DIR          "/workspaces/sdl_demo/build/ninja-mc/_deps/sdl_shadercross-src/external/DirectXShaderCompiler-binaries/linux"
                    BINARY_DIR          "/workspaces/sdl_demo/dxc_linux-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    USES_TERMINAL_PATCH     YES
)


