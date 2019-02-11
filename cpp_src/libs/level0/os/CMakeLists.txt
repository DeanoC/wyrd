set( LibName os)

set( CInterface
		os.h
		monitordesc.h
		rectdesc.h
		windowsdesc.h
		file.h
		filesystem.h
		)
set( CPPInterface
		platformevents.hpp
		thread.hpp
		file.hpp
		)

set( Src
		os.c
		file.cpp
		file_details.hpp

		)

if(WINDOWS)
	list(APPEND Src windows/file.cpp)
endif()

if(APPLE)
	list(APPEND Src apple/file.mm)
endif()

set( Deps
		level0/core
		level0/tinystl
		level0/math )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
