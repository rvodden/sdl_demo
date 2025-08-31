cmake_minimum_required(VERSION 3.21)

project(tictactoe LANGUAGES CXX VERSION 0.1.0)


add_subdirectory(data)

find_package(SDLXX REQUIRED)

# Has HEADER Files?
file( GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )
file( GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

if(SOURCE_FILES)
    foreach( SOURCE_FILE ${SOURCE_FILES} )
        message(DEBUG "   Found: ${SOURCE_FILE}" )
    endforeach()
endif()

if(HEADER_FILES)
    foreach( HEADER_FILE ${HEADER_FILES} )
        message(DEBUG "   Found: ${HEADER_FILE}" )
    endforeach()
endif()

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/ FILES ${HEADER_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME} PRIVATE standard_compiler_options)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_data)
target_link_libraries(${PROJECT_NAME} PUBLIC sdl::sdl)
target_link_libraries(${PROJECT_NAME} PUBLIC sdl::sdl_tools)
target_link_libraries(${PROJECT_NAME} PUBLIC sdl::sdl_application)
target_link_libraries(${PROJECT_NAME} PUBLIC sdl::sdl_main)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${PROJECT_NAME}
    FILE_SET generated_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${PROJECT_NAME}
)
