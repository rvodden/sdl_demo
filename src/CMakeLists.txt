set(CMAKE_CXX_CLANG_TIDY clang-tidy)

file(GLOB sources_list LIST_DIRECTORIES true *)
foreach(dir ${sources_list})
    if(IS_DIRECTORY ${dir} AND EXISTS "${dir}/CMakeLists.txt")
        add_subdirectory(${dir})
    endif()
endforeach()
