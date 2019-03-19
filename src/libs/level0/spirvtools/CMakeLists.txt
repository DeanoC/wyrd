
set(LibName spirvtools)

set(CInterface
        spirvtools.h
        )

set(CPPInterface
        )

set(Src
        spirvtools.cpp
        )

set(Deps
        level0/core)

set(SPIRV_SKIP_EXECUTABLES 1)
FetchContent_Declare(
        ThirdParty_SPIRVCross
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross
        GIT_TAG master
)
FetchContent_MakeAvailable(ThirdParty_SPIRVCross)
FetchContent_Declare(
        ThirdParty_SPIRVHeaders
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers
        GIT_TAG master
)
FetchContent_Declare(
        ThirdParty_SPIRVTools
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools
        GIT_TAG master
)
FetchContent_MakeAvailable(ThirdParty_SPIRVHeaders)
FetchContent_MakeAvailable(ThirdParty_SPIRVCross)
FetchContent_MakeAvailable(ThirdParty_SPIRVTools)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
target_link_libraries(${LibName} spirv-cross-cpp)
target_link_libraries(${LibName} SPIRV-Tools)
target_include_directories(${LibName} PRIVATE ${thirdparty_spirvtools_SOURCE_DIR}/include)