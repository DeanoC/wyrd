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
		quick_hash.hpp
		utils.hpp
		)
set(Src
		logger.c
		)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "" "")