set( LibName guishell)

set( CInterface
		guishell.h
		window.h
		)
set( CPPInterface
		guishell.hpp
		window.hpp
		)

set( Src
		guishell.cpp
		)
if (APPLE)
	list(APPEND Src apple/macapp.hpp)
	list(APPEND Src apple/macapp.mm)
endif()

set( Deps
		level0/core
		level0/os
		level0/tinystl
		)

set( Tests
		test_guishell.cpp )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_GUITESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}" "")

if (APPLE)
add_compile_definitions(${LibName} METAL)
target_compile_options(${LibName} PRIVATE -fobjc-arc)
target_link_libraries(${LibName} stdc++ "-framework Foundation" "-framework Cocoa" objc)
target_link_libraries(${LibName} "-framework Metal -framework MetalKit")
endif ()