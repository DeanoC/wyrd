set(LibName vfile)

set(CInterface
        interface.h
        vfile.h
        )

set(CPPInterface
        vfile.hpp)

set(Src
        vfile.c
        osfile.c
        osfile.h
        memfile.c
        memfile.h
        )

set(Deps
        level0/core
        level0/math
        level0/os
        level0/tinystl
        )

set(Tests
        test_vfile.cpp)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "${Tests}")