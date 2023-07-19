macro(standard_build)

    ## Library Build

    get_filename_component( LibraryName ${CMAKE_CURRENT_SOURCE_DIR} NAME )    
    
    message( CHECK_START "Loading ${LibraryName}..." )

    # Has HEADER Files?
    if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include )
        file( GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
    endif()

    if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src )
        file( GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
    endif()
    list(APPEND SOURCE_FILES ${HEADER_FILES})

    if( SOURCE_FILES )
        foreach( SOURCE_FILE ${SOURCE_FILES} )
            message( "   Adding file: ${SOURCE_FILE}" )
        endforeach()
        add_library( ${LibraryName} SHARED ${SOURCE_FILES} )

        target_include_directories( 
            ${LibraryName}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        )
    endif()
        
    message( CHECK_PASS "done." )
 
    ## Test Build

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

        target_link_libraries( ${TestName} PUBLIC ${LibraryName} )
        target_link_libraries( ${TestName} PUBLIC gtest )
        target_link_libraries( ${TestName} PUBLIC gmock )
        target_link_libraries( ${TestName} PUBLIC gtest_main )

        gtest_discover_tests( ${TestName} XML_OUTPUT_DIR report EXTRA_ARGS --gtest_catch_exceptions=0 )

        set(LibraryName ${LibraryName} PARENT_SCOPE)
    endif()

    # ## Documentation Build

    # set( DocName "${LibraryName}_docs" )
    
    # doxygen_add_docs( ${DocName} ${SOURCE_FILES} )
        
    # ## Benchmark Build

    # if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bench)
    #     set(BenchName "${LibraryName}_bench")
    #     file(GLOB_RECURSE BENCH_SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bench/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/bench/*.h)
    #     add_executable(${BenchName} ${BENCH_SOURCE_FILES})
    #     target_link_libraries(${BenchName} PUBLIC ${LibraryName})
    #     target_link_libraries(${BenchName} PRIVATE benchmark::benchmark_main)
    # endif()

endmacro()
