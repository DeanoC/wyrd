set(LibName stb)

set( CInterface
		stb_dxt.h
		stb_image.h
		stb_image_resize.h
		stb_image_write.h
		stb_truetype.h
		stb_dict.h )
set( CPPInterface
		)
set( Src stb_impl.c )

set( Deps level0/core )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "")
