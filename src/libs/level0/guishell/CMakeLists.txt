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

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "${Tests}")

if (APPLE)

add_compile_definitions(${LibName} METAL)
target_compile_options(${LibName} PRIVATE -fobjc-arc)

target_link_libraries(${LibName} stdc++ "-framework Foundation" "-framework Cocoa" objc)
target_link_libraries(${LibName} "-framework Metal")
set(RESOURCE_FILES src/apple/macresources/MainMenu.xib)

target_sources(${LibName}_tests PRIVATE "${RESOURCE_FILES}")
set_target_properties(${LibName}_tests PROPERTIES
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_GUI_IDENTIFIER "com.wryd.guishell_tests"
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/apple/macresources/Info.plist.in
		RESOURCE "${RESOURCE_FILES}"
		)
endif ()