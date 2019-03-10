set( LibName cmdlineshell)

set( CInterface
		cmdlineshell.h
		)
set( CPPInterface
		)

set( Src
		cmdlineshell.c
		)
set( Deps
		level0/core)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "")
if (APPLE)
    target_link_libraries(${LibName} stdc++ "-framework Foundation" "-framework Cocoa" objc)
endif ()