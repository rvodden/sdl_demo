standard_build()
add_dependency(${LibraryName} PRIVATE SDL2::SDL2)
add_dependency(${LibraryName} PRIVATE SDL2_image::SDL2_image)
add_dependency(${LibraryName} PRIVATE utils)

