set( LibName core)

set( CInterface
		platform.h
		platform_cuda.h
		platform_linux.h
		platform_osx.h
		platform_posix.h
		platform_win.h
		)

set( CPPInterface
		any.h
		core.h
		exception.h
		filesystem.h
		freelist.h
		linear_allocator.h
		quick_hash.h
		utils.h
		)
set(Src)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "")