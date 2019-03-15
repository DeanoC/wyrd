set( LibName os)

set( CInterface
		monitordesc.h
		rectdesc.h
		window.h
		file.h
		filesystem.h
        atomics.h
		)

set( CPPInterface
		platformevents.hpp
		thread.hpp
		file.hpp
        atomics.hpp
		)

set( Src
		file.c
		filesystem.cpp
		)

if (WIN32)
    list(APPEND Src windows/filesystem.cpp)
    list(APPEND Src windows/thread.c)
endif()

if(APPLE)
	list(APPEND Src posix/filesystem.cpp)
	list(APPEND Src apple/macwindow.mm)
	list(APPEND Src apple/window.mm)
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
