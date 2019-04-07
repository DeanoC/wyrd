set(LibName image)

set(CInterface
        image.h
        format.h
        format_cracker.h
        pixel.h
        io.h
        utils.h
        create.h
        )

set(CPPInterface
        image.hpp
        )

set(Src
        format_cracker.c
        block.c
        image.cpp
        fetch.hpp
        put.hpp
        loader.cpp
        saver.cpp
        utils.cpp
        convert.cpp
        create.cpp
        )

set(Deps
        level0/core
        level0/math
        level0/stb
        level1/vfile
        level2/syoyo
        )

set(Tests
        test_image.cpp
        test_pixel.cpp
        test_format_cracker.cpp
        test_image_io.cpp
        )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}" "")
