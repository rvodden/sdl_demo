cmake_minimum_required(VERSION 3.21)

project(snake LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_subdirectory(data)

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

# Create static library
add_library(${PROJECT_NAME}-static STATIC)
target_compile_features(${PROJECT_NAME}-static PUBLIC cxx_std_20)
target_sources(${PROJECT_NAME}-static PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/ FILES ${HEADER_FILES})
target_sources(${PROJECT_NAME}-static PRIVATE ${SOURCE_FILES})

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME}-static PRIVATE standard_compiler_options)
endif()

# target_link_libraries(${PROJECT_NAME}-static PUBLIC ${PROJECT_NAME}-data)
target_link_libraries(${PROJECT_NAME}-static PUBLIC sdl::sdl-static)
target_link_libraries(${PROJECT_NAME}-static PUBLIC sdl::sdl_tools-static)

# Add executable subdirectory
add_subdirectory(exe)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}-static EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${PROJECT_NAME}
)
