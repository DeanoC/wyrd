set(LibName vfile)

set(CInterface
        interface.h
        vfile.h
        utils.h
        )

set(CPPInterface
        vfile.hpp)

set(Src
        vfile.c
        osfile.c
        osfile.h
        memfile.c
        memfile.h
        utils.c
        )

set(Deps
        level0/core
        level0/math
        level0/os
        level0/tinystl
        )

set(Tests
        test_vfile.cpp)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")
