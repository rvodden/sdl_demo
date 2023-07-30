function(generateLibraryName Path)
    get_filename_component( LibraryName ${Path} NAME )    
    return(PROPAGATE LibraryName)
endfunction()

function (generateStaticNameFromLibraryName LibraryName)
    set(StaticName ${LibraryName}_static)
    return(PROPAGATE StaticName)
endfunction()

function(generateStaticName Path)
    generateLibraryName(${Path})
    generateStaticNameFromLibraryName(${LibraryName})
    return(PROPAGATE StaticName)
endfunction()

function(generateObjectNameFromLibraryName LibraryName)
    set(ObjectName ${LibraryName}_obj)
    return(PROPAGATE ObjectName)
endfunction()

function(generateObjectName Path)
    generateLibraryName(${Path})
    generateObjectNameFromLibraryName(${LibraryName})
    return(PROPAGATE ObjectName)
endfunction()

function (generateExecutableNameFromLibraryName LibraryName)
    set(ExecutableName ${LibraryName}_exe)
    return(PROPAGATE ExecutableName)
endfunction()

function(generateExecutableName Path)
    generateLibraryName(${Path})
    generateExecutableNameFromLibraryName(${LibraryName})
    return(PROPAGATE ExecutableName)
endfunction()

function(generateTestNameFromLibraryName Path)
    generateLibraryName(${Path})
    set(TestName ${LibraryName}_test)
    return(PROPAGATE TestName)
endfunction()

function(generateTestName Path)
    generateLibraryName(${Path})
    generateTestNameFromLibraryName(${LibraryName})
    return(PROPAGATE TestName)
endfunction()

function(get_sources Path Glob Role)
    if( EXISTS ${Path} )
        cmake_path(SET SearchPattern ${Path})
        cmake_path(APPEND SearchPattern ${Glob})
        file( GLOB_RECURSE Files CONFIGURE_DEPENDS ${SearchPattern})
    endif()
    
    if(Files)
        foreach( File ${Files} )
            message( "   Found ${Role} file: ${File}" )
        endforeach()
    endif()
    return(PROPAGATE Files)
endfunction()

function(getLibrarySources)
    get_sources(${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp source )
    set(LibraryFiles ${Files})
    return(PROPAGATE LibraryFiles)
endfunction()

function (getHeaderSources)
    get_sources(${CMAKE_CURRENT_SOURCE_DIR}/include *.h source )
    set(HeaderFiles ${Files})
    return(PROPAGATE HeaderFiles)
endfunction()

function (getTestSources)
    get_sources(${CMAKE_CURRENT_SOURCE_DIR}/test *.cpp source )
    set(TestFiles ${Files})
    return(PROPAGATE TestFiles)
endfunction()

function (getExecutableSources)
    get_sources(${CMAKE_CURRENT_SOURCE_DIR}/exe *.cpp source )
    set(ExecutableFiles ${Files})
    return(PROPAGATE ExecutableFiles)
endfunction()

function (addIncludeToTargetIncludePath Target Scope)
    target_include_directories( 
        ${Target}
        ${Scope}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
endfunction()

function (addSrcToTargetIncludePath Target Scope)
    target_include_directories( 
        ${Target}
        ${Scope}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    )
endfunction()

macro(addStaticAndSharedLibraryTargets)
    add_library( ${ObjectName} OBJECT ${LibraryFiles} )
    add_library( ${LibraryName} SHARED $<TARGET_OBJECTS:${ObjectName}> )
    add_library( ${StaticName} STATIC $<TARGET_OBJECTS:${ObjectName}> )
    set_target_properties(${StaticName} PROPERTIES OUTPUT_NAME ${LibraryName})
    
    addSrcToTargetIncludePath(${ObjectName} PRIVATE)

    addIncludeToTargetIncludePath(${ObjectName} PUBLIC)
    addIncludeToTargetIncludePath(${LibraryName} PUBLIC)
    addIncludeToTargetIncludePath(${StaticName} PUBLIC)

    set(LibraryName ${LibraryName} PARENT_SCOPE)
    set(ObjectName ${ObjectName} PARENT_SCOPE)
    set(StaticName ${StaticName} PARENT_SCOPE)
endmacro()

macro(addHeaderOnlyLibrary)
    add_library(${LibraryName} INTERFACE)
    addIncludeToTargetIncludePath(${LibraryName} INTERFACE)
    set(LibraryName ${LibraryName} PARENT_SCOPE)
endmacro()

macro(addLibraryTarget)
    message( CHECK_START "Loading ${LibraryName}..." )
    if( HeaderFiles AND NOT LibraryFiles )
        message("   Detected that ${LibraryName} is a header only library.")
        addHeaderOnlyLibrary()
    elseif( LibraryFiles )
        message("   ${LibraryName} has a library component.")
        addStaticAndSharedLibraryTargets()
    endif()
    message( CHECK_PASS "done." )
endmacro()

macro(addExecutableTarget)
    if( ExecutableFiles )
        add_executable( ${ExecutableName} ${ExecutableFiles} )
        target_link_libraries( ${ExecutableName} PUBLIC ${LibraryName} )
        
        set(ExecutableName ${ExecutableName} PARENT_SCOPE)
        set_target_properties(${ExecutableName} PROPERTIES OUTPUT_NAME ${LibraryName})
    endif()
endmacro()

macro(addTestTarget)
    if( TestFiles )
        if(NOT CMAKE_TESTING_ENABLED)
            include(GoogleTest)
            enable_testing()
        endif()

        add_executable ( ${TestName} ${TestFiles} )
        target_link_libraries( ${TestName} PRIVATE ${LibraryName} )
        target_link_libraries( ${TestName} PUBLIC gtest )
        target_link_libraries( ${TestName} PUBLIC gmock )
        target_link_libraries( ${TestName} PUBLIC gtest_main )

        addSrcToTargetIncludePath(${TestName} PRIVATE)
        gtest_discover_tests( ${TestName} XML_OUTPUT_DIR report EXTRA_ARGS --gtest_catch_exceptions=0 )
    endif()
endmacro()

macro(standard_build)
    generateLibraryName(${CMAKE_CURRENT_SOURCE_DIR})
    generateObjectName(${CMAKE_CURRENT_SOURCE_DIR})
    generateStaticName(${CMAKE_CURRENT_SOURCE_DIR})
    generateTestName(${CMAKE_CURRENT_SOURCE_DIR})
    generateExecutableName(${CMAKE_CURRENT_SOURCE_DIR})
    
    getHeaderSources()
    getLibrarySources()
    getExecutableSources()
    getTestSources()
    
    addLibraryTarget()
    addExecutableTarget()
    addTestTarget(${LibraryName})
endmacro()

function(add_dependency Target Scope Dependency)
    get_target_property(type ${Target} TYPE)
    if (${type} STREQUAL "SHARED_LIBRARY")
        generateStaticNameFromLibraryName(${Target})
        generateObjectNameFromLibraryName(${Target})
        target_link_libraries(${Target} ${Scope} ${Dependency})
        target_link_libraries(${StaticName} ${Scope} ${Dependency})
        target_link_libraries(${ObjectName} ${Scope} ${Dependency})
    else()
        target_link_libraries(${Target} ${Scope} ${Dependency})
    endif()
endfunction()

