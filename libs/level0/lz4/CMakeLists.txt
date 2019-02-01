set( Name lz4)
set( CInterface
		lz4.h
		lz4hc.h
		xxhash.h
		)
set( CPPInterface
		)

set( Src
		lz4.c
		lz4frame.c
		lz4frame.h
		lz4frame_static.h
		lz4hc.c
		xxhash.c
		)

ADD_LIB(${Name} "${CInterface}" "${CPPInterface}" "${Src}" "")
target_include_directories(${Name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/lz4)