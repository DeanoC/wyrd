set(LibName theforge)

set(CInterface
        renderer.h
        renderer_enums.h
        renderer_structs.h
        shader_reflection.h
        )

set(CPPInterface
        renderer.hpp
        )

set(Src
        )

if (APPLE)
    list(APPEND Src renderer_metal.hpp)
    list(APPEND Src renderer_metal.mm)
endif ()

set(Deps
        level0/core
        level0/os
        level0/tinystl
        )

set(Tests
        test_theforge.cpp
        )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "${Tests}")