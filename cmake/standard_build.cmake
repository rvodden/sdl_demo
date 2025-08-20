# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# Set common output directories to avoid DLL path issues on Windows
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


add_library(standard_compiler_options INTERFACE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(standard_compiler_options INTERFACE 
      -Werror
      -Wall
      -Wextra
      -Wpedantic
      -Wshadow
      -Wnon-virtual-dtor
      -Wold-style-cast
      -Wcast-align
      -Wunused
      -Woverloaded-virtual
      -Wconversion
      -Wsign-conversion
      -Wnull-dereference
      -Wdouble-promotion
      -Wformat=2
      -Wimplicit-fallthrough
      -Wmisleading-indentation
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  target_compile_options(standard_compiler_options INTERFACE
      -Werror
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn the user if a variable declaration shadows one from a parent context
      -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps
      # catch hard to track down memory errors
      -Wold-style-cast # warn for c-style casts
      -Wcast-align # warn for potential performance problem casts
      -Wunused # warn on anything being unused
      -Woverloaded-virtual # warn if you overload (not override) a virtual function
      -Wpedantic # warn if non-standard C++ is used
      -Wconversion # warn on type conversions that may lose data
      -Wsign-conversion # warn on sign conversions
      -Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicit promoted to double
      -Wformat=2 # warn on security issues around functions that format output (ie printf)
      -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
    )
endif()

# Enhance error reporting and compiler messages
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  if(WIN32)
    # On Windows cuda nvcc uses cl and not clang
    target_compile_options(standard_compiler_options INTERFACE $<$<COMPILE_LANGUAGE:C>:-fcolor-diagnostics> $<$<COMPILE_LANGUAGE:CXX>:-fcolor-diagnostics>)
  else()
    target_compile_options(standard_compiler_options INTERFACE -fcolor-diagnostics)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(WIN32)
    # On Windows cuda nvcc uses cl and not gcc
    target_compile_options($standard_compiler_options INTERFACE <$<COMPILE_LANGUAGE:C>:-fdiagnostics-color=always>
                        $<$<COMPILE_LANGUAGE:CXX>:-fdiagnostics-color=always>)
  else()
    target_compile_options(standard_compiler_options INTERFACE -fdiagnostics-color=always)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND MSVC_VERSION GREATER 1900)
  target_compile_options(standard_compiler_options INTERFACE /diagnostics:column)
else()
  message(STATUS "No colored compiler diagnostic set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

if(SDLCPP_USE_PRECOMPILED_HEADERS)
  add_library(precompiled_headers INTERFACE)
  target_precompile_headers(precompiled_headers
    INTERFACE
      $<BUILD_INTERFACE:<array>>
      $<BUILD_INTERFACE:<cmath>>
      $<BUILD_INTERFACE:<exception>>
      $<BUILD_INTERFACE:<format>>
      $<BUILD_INTERFACE:<forward_list>>
      $<BUILD_INTERFACE:<functional>>
      $<BUILD_INTERFACE:<iostream>>
      $<BUILD_INTERFACE:<ranges>>
  )
  set_target_properties(precompiled_headers PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
endif()
