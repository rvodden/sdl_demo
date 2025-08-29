#[[
    # Code Tests

    This module is designed to make it trival to add tests which configure and
    compile code against a library. There are three parts:

    - component installation
    - code tests
    - examples

    Component installation is used to install components created using the `cmake`
    `install` infrastructure into sandbox directories against which further tests
    can be run.

    Code tests are tests which consist of a `cmake` project; i.e. a directory
    with a `CMakeLists.txt` file and likely some source code. The code test
    infrastructure contained in this module will create ctest tests which configure,
    compile and run these projects.

    Examples are Code tests which are also added to a `cmake` `install` component
    and can therefore be run from the appropriate location within the sandbox
    installation.


#]]

#[[
    Using the function `install_component` looks like this:
    ```
        install_components(
            DIRECTORY ${CMAKE_BINARY_DIR}/place/to/install/component
            COMPONENTS component1 component2
        )
    ```

    Calling this function will register a pair of fixtures with `ctest`:

        - component1_installed
        - component2_installed

    Tests which include these fixtures in their `FIXTURES_REQUIRED` property
    will be guaranteed that the component `component1` will be installed in
    `${CMAKE_BINARY_DIR}/place/to/install/component/component1` and that
    `component2` will be installed in
    `${CMAKE_BINARY_DIR}/place/to/install/component/component2` respectively.

    The variables `component1_INSTALL_DIR` and `component2_INSTALL_DIR` will
    be set to the respective installation directories.

#]]
function(install_components)
    set(options "")
    set(oneValueArgs DIRECTORY)
    set(multiValueArgs COMPONENTS)
    cmake_parse_arguments(PARSE_ARGV 0 INSTALL_COMPONENTS "${options}" "${oneValueArgs}" "${multiValueArgs}")
    message(VERBOSE "Registering a fixture to install components: ${INSTALL_COMPONENTS_COMPONENTS}")

    if(NOT DEFINED INSTALL_COMPONENTS_DIRECTORY)
        set(INSTALL_COMPONENTS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/components")
        message(DEBUG "DIRECTORY not specified. Falling back on default of ${INSTALL_COMPONENTS_DIRECTORY}")
    endif()

    foreach(COMPONENT ${INSTALL_COMPONENTS_COMPONENTS})
        # Define a fixture to install the HDAS library
        string(TOUPPER ${COMPONENT} COMPONENT_UPPER)
        set(${COMPONENT_UPPER}_INSTALL_DIR "${INSTALL_COMPONENTS_DIRECTORY}/${COMPONENT}")
        set(${COMPONENT_UPPER}_INSTALL_DIR "${INSTALL_COMPONENTS_DIRECTORY}/${COMPONENT}" PARENT_SCOPE)
        message(DEBUG "Registering a fixture to install ${COMPONENT} to ${${COMPONENT_UPPER}_INSTALL_DIR}")
        add_test(
            NAME install_${COMPONENT}
            COMMAND ${CMAKE_COMMAND}
            --install .
            --prefix ${${COMPONENT_UPPER}_INSTALL_DIR}
            --component ${COMPONENT}
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            COMMAND_EXPAND_LISTS
        )
        set_tests_properties(install_${COMPONENT} PROPERTIES FIXTURES_SETUP ${COMPONENT}_installed)

        # Define a fixture to uninstall the HDAS library
        message(DEBUG "Registering a fixture to delete the ${${COMPONENT_UPPER}_INSTALL_DIR} directory")
        add_test(
            NAME remove_${COMPONENT}
            COMMAND ${CMAKE_COMMAND}
                -E remove_directory ${${COMPONENT_UPPER}_INSTALL_DIR}
        )
        set_tests_properties(remove_${COMPONENT} PROPERTIES FIXTURES_CLEANUP ${COMPONENT}_installed)
    endforeach()
endfunction()

function(install_package)
    set(options "")
    set(oneValueArgs NAME DIRECTORY)
    set(multiValueArgs COMPONENTS)
    cmake_parse_arguments(PARSE_ARGV 0 INSTALL_PACKAGE "${options}" "${oneValueArgs}" "${multiValueArgs}")
    if(DEFINED INSTALL_PACKAGE_COMPONENTS)
        message(VERBOSE "Registering a fixture to install project as a package named ${INSTALL_PACKAGE_NAME} consisting of components: ${INSTALL_PACKAGE_COMPONENTS}")
    else()
        message(VERBOSE "Registering a fixture to install project as a package named ${INSTALL_PACKAGE_NAME}.")
    endif()

    if(NOT DEFINED INSTALL_PACKAGE_DIRECTORY)
        set(INSTALL_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${INSTALL_PACKAGE_NAME}")
        message(DEBUG "DIRECTORY not specified. Falling back on default of ${INSTALL_PACKAGE_DIRECTORY}")
    endif()

    string(TOUPPER ${INSTALL_PACKAGE_NAME} PACKAGE_UPPER)
    set(${PACKAGE_UPPER}_INSTALL_DIR "${INSTALL_PACKAGE_DIRECTORY}")
    set(${PACKAGE_UPPER}_INSTALL_DIR "${INSTALL_PACKAGE_DIRECTORY}" PARENT_SCOPE)
    
    # if components is not defined, then just install the entire project
    if(NOT DEFINED INSTALL_PACKAGE_COMPONENTS)
        # Define a fixture to install the package
        message(DEBUG "Registering a fixture to install ${INSTALL_PACKAGE_NAME} to ${${PACKAGE_UPPER}_INSTALL_DIR}")
        add_test(
            NAME CodeTest.install_${INSTALL_PACKAGE_NAME}
            COMMAND ${CMAKE_COMMAND}
            --install .
            --prefix ${${PACKAGE_UPPER}_INSTALL_DIR}
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            COMMAND_EXPAND_LISTS
        )
        set_tests_properties(CodeTest.install_${INSTALL_PACKAGE_NAME} PROPERTIES FIXTURES_SETUP CodeTest.${INSTALL_PACKAGE_NAME}_installed)

        # Define a fixture to uninstall the package
        message(DEBUG "Registering a fixture to delete the ${${PACKAGE_UPPER}_INSTALL_DIR} directory")
        add_test(
            NAME CodeTest.remove_${INSTALL_PACKAGE_NAME}
            COMMAND ${CMAKE_COMMAND}
                -E remove_directory ${${PACKAGE_UPPER}_INSTALL_DIR}
        )
        set_tests_properties(CodeTest.remove_${INSTALL_PACKAGE_NAME} PROPERTIES FIXTURES_CLEANUP CodeTest.${INSTALL_COMPONENT_NAME}_installed)
    else()
        foreach(COMPONENT ${INSTALL_PACKAGE_COMPONENTS})
            # defin a fixture to install the component
            message(DEBUG "Registering a fixture to install ${COMPONENT} to ${${PACKAGE_UPPER}_INSTALL_DIR}")
            add_test(
                NAME CodeTest.install_${COMPONENT}
                COMMAND ${CMAKE_COMMAND}
                --install .
                --prefix ${${PACKAGE_UPPER}_INSTALL_DIR}
                --component ${COMPONENT}
                WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                COMMAND_EXPAND_LISTS
            )
            set_tests_properties(CodeTest.install_${COMPONENT} PROPERTIES FIXTURES_SETUP CodeTest.${INSTALL_PACKAGE_NAME}_installed)
            
        endforeach()
        
        # Define a fixture to uninstall the package
        message(DEBUG "Registering a fixture to delete the ${${PACKAGE_UPPER}_INSTALL_DIR} directory")
        add_test(
            NAME CodeTest.remove_${INSTALL_PACKAGE_NAME}
            COMMAND ${CMAKE_COMMAND}
                -E remove_directory ${${PACKAGE_UPPER}_INSTALL_DIR}
        )
        set_tests_properties(CodeTest.remove_${INSTALL_PACKAGE_NAME} PROPERTIES FIXTURES_CLEANUP ${INSTALL_PACKAGE_NAME}_installed)

    endif()

endfunction()

#[[
    Using the function `register_code_test` looks like this:
    ```
        register_code_test(
            NAME code_test_name
            [SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/code_test_name]
            [BUILD_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/code_test_name]
            [COMPONENTS component1 component2]
        )
    ```

    If SOURCE_DIRECTORY is ommitted it will default to ${CMAKE_CURRNET_SOURCE_DIR}/code_test_name
    if BUILD_DIRECTORY is omitted it will default to ${CMAKE_CURRENT_BINARY_DIR}/code_test_name

    This function creates a pair of fixtures: `create_code_test_name_build_directory` and `remove_code_test_name_build_directory` which
    respectively create and remove the build directory specified when the function was called, or the default if ommitted.

    It the creates two (soon to be three) tests:
#]]
function(register_code_test)
    set(options "")
    set(oneValueArgs NAME SOURCE_DIRECTORY BUILD_DIRECTORY)
    set(multiValueArgs COMPONENTS USE_FETCHED_DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 0 REGISTER_CODE_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}")
    message(VERBOSE "Registering code test: ${REGISTER_CODE_TEST_NAME}")

    if(NOT DEFINED REGISTER_CODE_TEST_SOURCE_DIRECTORY)
        set(REGISTER_CODE_TEST_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${REGISTER_CODE_TEST_NAME})
    endif()

    if(NOT DEFINED REGISTER_CODE_TEST_BUILD_DIRECTIRY)
        set(REGISTER_CODE_TEST_BUILD_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${REGISTER_CODE_TEST_NAME})
    endif()

    # Define a fixture to create the build directory for the code test
    add_test(
        NAME CodeTest.create_${REGISTER_CODE_TEST_NAME}_directory
        COMMAND ${CMAKE_COMMAND} -E make_directory ${REGISTER_CODE_TEST_BUILD_DIRECTORY}
    )
    set_tests_properties(CodeTest.create_${REGISTER_CODE_TEST_NAME}_directory PROPERTIES FIXTURES_SETUP CodeTest.${REGISTER_CODE_TEST_NAME}_directory)

    # Define a fixture to remove the build directory for the code test
    add_test(
        NAME CodeTest.remove_${REGISTER_CODE_TEST_NAME}_directory
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${REGISTER_CODE_TEST_BUILD_DIRECTORY}
    )
    set_tests_properties(CodeTest.remove_${REGISTER_CODE_TEST_NAME}_directory PROPERTIES FIXTURES_CLEANUP CodeTest.${REGISTER_CODE_TEST_NAME}_directory)

    foreach(DEPENDENCY ${REGISTER_CODE_TEST_USE_FETCHED_DEPENDENCIES})
        message(VERBOSE "Associating ${DEPENDENCY} with ${REGISTER_CODE_TEST_NAME}")
        if(NOT DEFINED ${DEPENDENCY}_BINARY_DIR)
            message(FATAL_ERROR "Do not know where to find source for dependency ${DEPENDENCY}, required by CodeTest.${REGISTER_CODE_TEST_NAME}.")
        endif()
        list(APPEND ARGUMENT_LIST "-D${DEPENDENCY}_DIR=${${DEPENDENCY}_BINARY_DIR}")
    endforeach()

    foreach(COMPONENT ${REGISTER_CODE_TEST_COMPONENTS})
        message(VERBOSE "Associating ${COMPONENT} with ${REGISTER_CODE_TEST_NAME}")
        string(TOUPPER ${COMPONENT} COMPONENT_UPPER)
        if(NOT DEFINED ${COMPONENT_UPPER}_INSTALL_DIR)
            message(FATAL_ERROR "Do not know where ${COMPONENT} is installed, required by ${REGISTER_CODE_TEST_NAME}.")
        endif()
        list(APPEND COMPONENT_ARGUMENT_LIST "-D${COMPONENT_UPPER}_DIR=${${COMPONENT_UPPER}_INSTALL_DIR}/lib/cmake/${COMPONENT_UPPER}")
        list(APPEND FIXTURES_REQUIRED "CodeTest.${COMPONENT}_installed")
    endforeach()
    string(JOIN ";" FIXTURES_REQUIRED_STRING ${FIXTURES_REQUIRED})

    if(DEFINED CMAKE_TOOLCHAIN_FILE)
        list(APPEND ARGUMENT_LIST "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    endif()

    # Create a test which runs `cmake` in configure mode
    add_test(
        NAME CodeTest.test_${REGISTER_CODE_TEST_NAME}_configures
        COMMAND ${CMAKE_COMMAND}
            -G "${CMAKE_GENERATOR}"
            ${ARGUMENT_LIST}
            -DCMAKE_MODULE_PATH=${CMAKE_SOURCE_DIR}/cmake
            ${COMPONENT_ARGUMENT_LIST}
            ${REGISTER_CODE_TEST_SOURCE_DIRECTORY}
        WORKING_DIRECTORY ${REGISTER_CODE_TEST_BUILD_DIRECTORY}
        COMMAND_EXPAND_LISTS
    )
    # Configuration requires HDAS to be installed and requires the build directory
    set_tests_properties(CodeTest.test_${REGISTER_CODE_TEST_NAME}_configures PROPERTIES FIXTURES_REQUIRED "${FIXTURES_REQUIRED_STRING};CodeTest.${REGISTER_CODE_TEST_NAME}_directory")
    # Declare this test as a fixture so that compilation can be declared after it
    set_tests_properties(CodeTest.test_${REGISTER_CODE_TEST_NAME}_configures PROPERTIES FIXTURES_SETUP CodeTest.${REGISTER_CODE_TEST_NAME}_configured)

    # Create a test which runs `cmake` in build mode
    add_test(
        NAME CodeTest.test_${REGISTER_CODE_TEST_NAME}_compiles
        COMMAND ${CMAKE_COMMAND}
            --build
            .
        WORKING_DIRECTORY ${REGISTER_CODE_TEST_BUILD_DIRECTORY}
    )
    # Compilation requires HDAS to be installed, requires the build directory, and requires configuration to have happened.
    set_tests_properties(CodeTest.test_${REGISTER_CODE_TEST_NAME}_compiles PROPERTIES FIXTURES_REQUIRED "${FIXTURES_REQUIRED_STRING};CodeTest.${REGISTER_CODE_TEST_NAME}_directory;CodeTest.${REGISTER_CODE_TEST_NAME}_configured")

endfunction()
