# Tictactoe executable
add_executable(${PROJECT_NAME}_exe main.cpp)
target_compile_features(${PROJECT_NAME}_exe PUBLIC cxx_std_20)

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(${PROJECT_NAME}_exe PRIVATE standard_compiler_options)
endif()

# Link against the static library and dependencies
target_link_libraries(${PROJECT_NAME}_exe PRIVATE ${PROJECT_NAME}-static)
target_link_libraries(${PROJECT_NAME}_exe PRIVATE sdl::sdl_main-static)

# Set the output name to be the same as the project name (backward compatibility)
set_target_properties(${PROJECT_NAME}_exe PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

# Installation
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}_exe 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME}
)