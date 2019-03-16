set( LibName guishell)

set( CInterface
		guishell.h
		)
set( CPPInterface
		guishell.hpp)

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
ADD_LIB_GUITESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")

if (APPLE)
add_compile_definitions(${LibName} METAL)
target_compile_options(${LibName} PRIVATE -fobjc-arc)
target_link_libraries(${LibName} stdc++ "-framework Foundation" "-framework Cocoa" objc)
target_link_libraries(${LibName} "-framework Metal -framework MetalKit")

set(RESOURCE_FILES MainMenu)
# Compile the .xib files using the 'ibtool' program with the destination being the app package
FOREACH( xib ${RESOURCE_FILES} )
	ADD_CUSTOM_COMMAND( TARGET ${LibName}_tests PRE_BUILD
			COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text
			--compile ${CMAKE_CURRENT_SOURCE_DIR}/src/apple/macresources/MainMenu.nib
			${CMAKE_CURRENT_SOURCE_DIR}/src/apple/macresources/MainMenu.xib
			COMMENT "Compiling ${xib}.xib")
ENDFOREACH()

endif ()