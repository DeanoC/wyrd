set(LibName theforge)

set(CInterface
        renderer.h
        renderer_enums.h
        renderer_structs.h
        shader_reflection.h
        )

set(CPPInterface
        )

set(Src
        IRay.h
        IRenderer.h
        ResourceLoader.h
        ResourceLoader.cpp
        Image/Image.h
        Image/ImageEnums.h)

if (APPLE)
    list(APPEND Src Metal/MetalMemoryAllocator.h)
    list(APPEND Src Metal/MetalRaytracing.mm)
    list(APPEND Src Metal/MetalRenderer.mm)
    list(APPEND Src Metal/MetalShaderReflection.mm)
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