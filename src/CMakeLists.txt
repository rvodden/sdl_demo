
file(GLOB sources_list LIST_DIRECTORIES true *)
foreach(dir ${sources_list})
    if(IS_DIRECTORY ${dir} AND EXISTS "${dir}/CMakeLists.txt")
        add_subdirectory(${dir})
    endif()
endforeach()
