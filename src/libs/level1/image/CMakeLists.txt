set(LibName image)

set(CInterface
        image.h
        format.h
        format_cracker.h
        pixel.h
        )

set(CPPInterface
        )

set(Src
        format_cracker.c
        block.c
        image.cpp
        fetch.hpp
        put.hpp
        loader.cpp
        )

set(Deps
        level0/core
        level0/math
        level0/os
        level0/tinystl
        level0/stb
        level1/vfile
        )

set(Tests
        test_image.cpp
        test_pixel.cpp
        test_format_cracker.cpp
        )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")
