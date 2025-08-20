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
elseif(MSVC)
  target_compile_options(standard_compiler_options INTERFACE
    /W4 # Baseline reasonable warnings
    /w14242 # 'identifier': conversion from 'type1' to 'type2', possible loss of data
    /w14254 # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
    /w14263 # 'function': member function does not override any base class virtual member function
    /w14265 # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not
            # be destructed correctly
    /w14287 # 'operator': unsigned/negative constant mismatch
    /we4289 # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside
            # the for-loop scope
    /w14296 # 'operator': expression is always 'boolean_value'
    /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
    /w14545 # expression before comma evaluates to a function which is missing an argument list
    /w14546 # function call before comma missing argument list
    /w14547 # 'operator': operator before comma has no effect; expected operator with side-effect
    /w14549 # 'operator': operator before comma has no effect; did you intend 'operator'?
    /w14555 # expression has no effect; expected expression with side- effect
    /w14619 # pragma warning: there is no warning number 'number'
    /w14640 # Enable warning on thread un-safe static member initialization
    /w14826 # Conversion from 'type1' to 'type2' is sign-extended. This may cause unexpected runtime behavior.
    /w14905 # wide string literal cast to 'LPSTR'
    /w14906 # string literal cast to 'LPWSTR'
    /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
    /wd4275 # prevent exporting classes which inherit from non-exported classes (breaks exceptions)
    /wd4251 # prevent exported classes having members which are non-exported classed (break pimpl)
    /wd4068 # unknown pragma (the point of pragmas is that they might be unknown!) 
    /permissive- # standards conformance mode for MSVC compiler. 
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
