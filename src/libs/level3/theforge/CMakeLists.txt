set(LibName theforge)

set(CInterface
        renderer.h
        raytracer.h
        renderer_enums.h
        renderer_structs.h
        )

set(CPPInterface
        renderer.hpp
        raytracer.hpp
        renderer_enums.hpp
        renderer_structs.hpp
        )

set(Src
        c_api.cpp
        utils.hpp
        dynamic_memory_allocator.cpp
        dynamic_memory_allocator.hpp
        )

if (APPLE)
    list(APPEND Src metal/cpp_api.mm)
    list(APPEND Src metal/cmds.mm)
    list(APPEND Src metal/descriptor.mm)
    list(APPEND Src metal/memory_allocator.hpp)
    list(APPEND Src metal/raytracing.mm)
    list(APPEND Src metal/renderer.mm)
    list(APPEND Src metal/utils.mm)

    list(APPEND CPPInterface metal/descriptor.hpp)
    list(APPEND CPPInterface metal/renderer.hpp)
    list(APPEND CPPInterface metal/structs.hpp)
    list(APPEND CPPInterface metal/utils.hpp)
endif ()

set(Deps
        level0/core
        level0/os
        level0/tinystl
        level0/stb
        level1/vfile
        level2/image
        )

set(Tests
        test_theforge.cpp
        )

set(TestsDep
        level1/logmanager
    )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}" "${TestsDep}")

if (APPLE)
    add_compile_definitions(${LibName} METAL)
    target_compile_options(${LibName} PRIVATE -fobjc-arc)
    target_link_libraries(${LibName} "-framework Metal -framework IoKit -framework MetalPerformanceShaders")
endif()