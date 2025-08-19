include(CMakeDependentOption)

option(SDLCPP_USE_PRECOMPILED_HEADERS "Use pre-compiled headers (can improve build time)" ON)

cmake_dependent_option(
    SDLCPP_USE_CLANG_TIDY 
    "Inspect code with clang-tidy (incompatible with pre-compiled headers on non-clang compilers)" 
    ON
    [[ (NOT SDLCPP_USE_PRECOMPILED_HEADERS) OR (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang") ]]
    OFF
)

