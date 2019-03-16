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
        image.cpp
        image_fetch.hpp
        image_put.hpp
        )

set(Deps
        level0/core
        level0/math
        level0/os
        level0/tinystl
        )

set(Tests
        test_image.cpp
        test_pixel.cpp
        test_format_cracker.cpp
        )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")
