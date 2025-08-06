standard_build()
set_target_properties(${LibraryName} PROPERTIES LINKER_LANGUAGE CXX)

find_library(MATH_LIBRARY m)
target_link_libraries(${LibraryName} PRIVATE ${MATH_LIBRARY})
