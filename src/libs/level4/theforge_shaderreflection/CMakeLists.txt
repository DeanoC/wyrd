set(LibName theforge_shaderreflection)

set(CInterface
        theforge_shaderreflection.h
        )

set(CPPInterface
        theforge_shaderreflection.hpp
        )

set(Src
        c_api.cpp
        theforge_shaderreflection.cpp
        )

if (APPLE)
    list(APPEND Src metal/theforge_shaderreflection.mm)
endif ()

set(Deps
        level0/core
        level0/os
        level0/tinystl
        level1/vfile
        level3/theforge
        )

set(Tests
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