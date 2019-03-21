set(LibName syoyo)

set( CInterface
        tiny_objloader.h
 )
set( CPPInterface
        tiny_xpd.hpp
        )
set( Src
        syoyo_impl.cpp
        syoyo_impl.c
        tiny_objloader.c
        )

set( Deps
        level0/core
        level0/tinystl
        level1/vfile
        )

set( Tests
        test_objloader_c.cpp)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")
