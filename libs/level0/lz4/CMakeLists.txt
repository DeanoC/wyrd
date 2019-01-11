set( Name lz4)
set( Interface
		)

set( Impl
		lz4.c
		lz4.h
		lz4frame.c
		lz4frame.h
		lz4frame_static.h
		lz4hc.c
		lz4hc.h
		xxhash.c
		xxhash.h
		)

ADD_LIB(${Name} ${Interface} ${Impl} )