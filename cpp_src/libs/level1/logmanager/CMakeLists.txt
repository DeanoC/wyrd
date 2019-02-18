set( LibName logmanager)

set( CInterface
		)
set( CPPInterface
		)

set( Src
		LogManager.cpp
		LogManager.h
		)
set( Deps
		level0/core
		level0/math
		level0/os
		level0/tinystl
		)


ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "")