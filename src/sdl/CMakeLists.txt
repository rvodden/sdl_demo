standard_build()
target_link_libraries(${LibraryName} PRIVATE SDL3::SDL3)
target_link_libraries(${LibraryName} PRIVATE SDL3_image::SDL3_image)
target_link_libraries(${LibraryName} PRIVATE utils)

find_library(MATH_LIBRARY m)
target_link_libraries(${LibraryName} PRIVATE ${MATH_LIBRARY})
