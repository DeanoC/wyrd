set(LibName syoyo)

set( CInterface
        tiny_objloader.h
        tiny_exr.h
 )
set( CPPInterface
        tiny_xpd.hpp
        tiny_exr.hpp
        )
set( Src
        syoyo_impl.cpp
        syoyo_impl.c
        tiny_objloader.c
        tinyexr.cpp
        tinyexr.hpp
        tinyexr_bindings.cpp
        )

set( Deps
        level0/core
        level0/tinystl
        level0/miniz
        level1/vfile
        )

set( Tests
        test_syoyo.cpp
        test_objloader_c.cpp
        test_exr_c.cpp)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}" "")
