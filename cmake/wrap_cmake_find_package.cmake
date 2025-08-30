set(_BUILT_PACKAGES
    SDLXX
)

macro(find_package)
    if(NOT ${ARGV0} IN_LIST _BUILT_PACKAGES)
        _find_package(${ARGV})
    endif()
endmacro()
