set( LibName core)

set( CInterface
		core.h
		platform.h
		platform_linux.h
		platform_osx.h
		platform_posix.h
		platform_win.h
		)

set( CPPInterface
		any.h
		exception.h
		filesystem.h
		freelist.h
		linear_allocator.h
		quick_hash.h
		utils.h
		)
set(Src
		logger.c
		)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "" "")