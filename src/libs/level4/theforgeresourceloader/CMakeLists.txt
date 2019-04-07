set(LibName theforgeresourceloader)

set(CInterface
        resourceloader.h
        )

set(CPPInterface
        )

set(Src
        resource_loader.h
        resource_loader.cpp
        )

if (APPLE)
endif ()

set(Deps
        level0/core
        level0/os
        level0/tinystl
        level0/stb
        level2/image
        level3/theforge
        )

set(Tests
        )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
#ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")

if (APPLE)
    add_compile_definitions(${LibName} METAL)
    target_compile_options(${LibName} PRIVATE -fobjc-arc)
    target_link_libraries(${LibName} "-framework Metal -framework IoKit -framework MetalPerformanceShaders")
endif()