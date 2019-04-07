set(LibName theforge_display)

set(CInterface
        theforge_display.h
        )

set(CPPInterface
        theforge_display.hpp
        )

set(Src
        display.cpp
        c_api.cpp
        )

if (APPLE)
    list(APPEND Src metal/metal.mm)
    list(APPEND Src metal/cpp_api.mm)

    list(APPEND CPPInterface metal/metal.hpp)

endif ()

set(Deps
        level0/core
        level0/guishell
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