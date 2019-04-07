set(LibName theforge_resourceloader)

set(CInterface
        theforge_resourceloader.h
        )

set(CPPInterface
        theforge_resourceloader.hpp
        )

set(Src
        resource_loader.cpp
        c_api.cpp
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
        test_theforgeresourceloader.cpp
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