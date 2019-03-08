set( LibName logmanager)

set( CInterface
		logmanager.h
		)

set(CPPInterface
		logmanager.hpp
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