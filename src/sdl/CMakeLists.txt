standard_build()
target_link_libraries(${LibraryName} PRIVATE SDL2::SDL2)
target_link_libraries(${LibraryName} PRIVATE SDL2_image::SDL2_image)
target_link_libraries(${LibraryName} PRIVATE utils)

