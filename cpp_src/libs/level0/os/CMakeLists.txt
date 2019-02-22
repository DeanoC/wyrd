set( LibName os)

set( CInterface
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
		file.c
		filesystem.cpp
		)

if(WINDOWS)
	list(APPEND Src windows/file.cpp)
endif()

if(APPLE)
	list(APPEND Src posix/filesystem.cpp)
	list(APPEND Src apple/filesystem.mm)
	list(APPEND Src posix/thread.c)
endif()

set( Deps
		level0/core
		level0/tinystl
		level0/math )

set(Tests
		test_os.cpp
		test_file.cpp
		test_filesystem.cpp
		test_thread.cpp)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "${Tests}")
