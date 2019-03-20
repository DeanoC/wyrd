set(LibName theforge)

set(CInterface
        renderer.h
        raytracer.h
        renderer_enums.h
        renderer_structs.h
        shader_reflection.h
        image_enums.h
        )

set(CPPInterface
        renderer.hpp
        raytracer.hpp
        renderer_enums.hpp
        renderer_structs.hpp
        shader_reflection.hpp
        image_enums.hpp
        )

set(Src
        c_api.cpp
        shader_reflection.cpp
        Image/image.cpp
        Image/image.h
        utils.hpp
        dynamic_memory_allocator.cpp
        dynamic_memory_allocator.hpp
        )

if (APPLE)
    list(APPEND Src Metal/cpp_api.mm)
    list(APPEND Src Metal/cmds.mm)
    list(APPEND Src Metal/descriptor_manager.hpp)
    list(APPEND Src Metal/descriptor_manager.mm)
    list(APPEND Src Metal/memory_allocator.hpp)
    list(APPEND Src Metal/raytracing.mm)
    list(APPEND Src Metal/renderer.hpp)
    list(APPEND Src Metal/renderer.mm)
    list(APPEND Src Metal/shader_reflection.mm)
    list(APPEND Src Metal/structs.hpp)
    list(APPEND Src Metal/utils.hpp)
    list(APPEND Src Metal/utils.mm)
endif ()

set(Deps
        level0/core
        level0/os
        level0/tinystl
        level0/stb
        level1/guishell
        level1/vfile
        level1/image
        )

set(Tests
        test_theforge.cpp
        )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")

if (APPLE)
    add_compile_definitions(${LibName} METAL)
    target_compile_options(${LibName} PRIVATE -fobjc-arc)
    target_link_libraries(${LibName} "-framework Metal -framework IoKit -framework MetalPerformanceShaders")
endif()