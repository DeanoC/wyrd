set(LibName theforge)

set(CInterface
        renderer.h
        raytracer.h
        renderer_enums.h
        renderer_structs.h
        shader_reflection.h
        )

set(CPPInterface
        renderer.hpp
        raytracer.hpp
        renderer_enums.hpp
        renderer_structs.hpp
        shader_reflection.hpp
        )

set(Src
        c_api.cpp
        shader_reflection.cpp
        utils.hpp
        dynamic_memory_allocator.cpp
        dynamic_memory_allocator.hpp
        )

if (APPLE)
    list(APPEND Src metal/cpp_api.mm)
    list(APPEND Src metal/cmds.mm)
    list(APPEND Src metal/descriptor_manager.mm)
    list(APPEND Src metal/memory_allocator.hpp)
    list(APPEND Src metal/raytracing.mm)
    list(APPEND Src metal/renderer.mm)
    list(APPEND Src metal/shader_reflection.mm)
    list(APPEND Src metal/utils.mm)


    list(APPEND CPPInterface metal/descriptor_manager.hpp)
    list(APPEND CPPInterface metal/renderer.hpp)
    list(APPEND CPPInterface metal/structs.hpp)
    list(APPEND CPPInterface metal/utils.hpp)
endif ()

if(WIN32)
#TODO pick API

#Vulkan
FetchContent_Declare(
        ThirdParty_VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG master
)
FetchContent_MakeAvailable(ThirdParty_VulkanMemoryAllocator)

list(APPEND Src vulkan/raytracing.cpp)
list(APPEND Src vulkan/renderer.cpp)
list(APPEND Src vulkan/shader_reflection.cpp)
list(APPEND CPPInterface vulkan/descriptor_manager.hpp)
list(APPEND CPPInterface vulkan/renderer.hpp)
list(APPEND CPPInterface vulkan/structs.hpp)
list(APPEND CPPInterface vulkan/utils.hpp)
endif()

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
if(WIN32)
    target_include_directories(${LibName} PRIVATE ${thirdparty_vulkanmemoryallocator_SOURCE_DIR}/src)
endif()