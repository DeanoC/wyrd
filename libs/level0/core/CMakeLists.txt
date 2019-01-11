set(LibName core)

set( Interface
		any.h
		core.h
		exception.h
		filesystem.h
		freelist.h
		linear_allocator.h
		loguru.hpp
		platform.h
		platform_cuda.h
		platform_linux.h
		platform_osx.h
		platform_posix.h
		platform_win.h
		quick_hash.h
		utils.h
		)
set( Src )

ADD_LIB(${LibName} ${Interface} ${Src})