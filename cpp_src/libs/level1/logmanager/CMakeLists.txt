set( LibName logmanager)

set( CInterface
		)

set( CPPInterface
        logmanager.h
		)

set( Src
        logmanager.cpp
        )

set( Deps
		level0/core
		level0/math
		level0/os
		level0/tinystl
		)

set(Tests
        test_logmanager.cpp
        )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "${Tests}")