add_library(standard_compiler_options INTERFACE)
target_compile_options(standard_compiler_options INTERFACE -Werror -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough -Wmisleading-indentation)
target_link_libraries(standard_compiler_options INTERFACE stdc++)

macro(get_sources)

    # Has HEADER Files?
    if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include )
        file( GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
    endif()

    if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src )
        file( GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
    endif()
    list(APPEND SOURCE_FILES ${HEADER_FILES})

    if(SOURCE_FILES)
        foreach( SOURCE_FILE ${SOURCE_FILES} )
            message( "   Found: ${SOURCE_FILE}" )
        endforeach()
    endif()
endmacro()

macro(add_test_target)
    if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test )
        if(NOT CMAKE_TESTING_ENABLED)
            include(GoogleTest)
            enable_testing()
            set( TestName "${LibraryName}_test" )
        endif()

        file( GLOB_RECURSE TEST_SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/test/*.h )
        foreach( SOURCE_FILE ${TEST_SOURCE_FILES} )
            message( "   Adding test source file: ${SOURCE_FILE}" )
        endforeach()
        add_executable( ${TestName} ${TEST_SOURCE_FILES} )

        target_link_libraries( ${TestName} PRIVATE standard_compiler_options )
        target_link_libraries( ${TestName} PUBLIC ${LibraryName} )
        target_link_libraries( ${TestName} PUBLIC gtest )
        target_link_libraries( ${TestName} PUBLIC gmock )
        target_link_libraries( ${TestName} PUBLIC gtest_main )

        gtest_discover_tests( ${TestName} XML_OUTPUT_DIR report EXTRA_ARGS --gtest_catch_exceptions=0 )

        set(LibraryName ${LibraryName} PARENT_SCOPE)
    endif()
endmacro()

macro(standard_libarary_build)

    ## Library Build

    get_filename_component( LibraryName ${CMAKE_CURRENT_SOURCE_DIR} NAME )    
    
    message( CHECK_START "Loading ${LibraryName}..." )
    get_sources()

    if( SOURCE_FILES )    
        add_library( ${LibraryName} SHARED ${SOURCE_FILES} )

        target_include_directories( 
            ${LibraryName}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        )

        target_link_libraries(${LibraryName} PRIVATE standard_compiler_options)
    endif()
        
    message( CHECK_PASS "done." )

    add_test_target()

endmacro()

macro(standard_executable_build)

    ## Executable Build

    get_filename_component( LibraryName ${CMAKE_CURRENT_SOURCE_DIR} NAME )    
    
    message( CHECK_START "Loading ${LibraryName}..." )
    get_sources()

    if( SOURCE_FILES )    
        add_executable( ${LibraryName} ${SOURCE_FILES} )

        target_include_directories( 
            ${LibraryName}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
        
        target_link_libraries(${LibraryName} PRIVATE standard_compiler_options)
    endif()
        
    message( CHECK_PASS "done." )

    add_test_target()

endmacro()

macro(standard_build)
    standard_libarary_build()
endmacro()


