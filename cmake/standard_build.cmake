add_library(standard_compiler_options INTERFACE)
target_compile_options(standard_compiler_options INTERFACE -Werror -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough -Wmisleading-indentation)
target_link_libraries(standard_compiler_options INTERFACE stdc++)
