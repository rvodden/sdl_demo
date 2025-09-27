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

project(dxc_windows-populate NONE)



include(ExternalProject)
ExternalProject_Add(dxc_windows-populate
                     "URL" "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2505/dxc_2025_05_24.zip" "URL_HASH" "SHA256=81380f3eca156d902d6404fd6df9f4b0886f576ff3e18b2cc10d3075ffc9d119"
                    SOURCE_DIR          "/workspaces/sdl_demo/build/ninja-mc/_deps/sdl_shadercross-src/external/DirectXShaderCompiler-binaries/windows"
                    BINARY_DIR          "/workspaces/sdl_demo/dxc_windows-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    USES_TERMINAL_PATCH     YES
)


