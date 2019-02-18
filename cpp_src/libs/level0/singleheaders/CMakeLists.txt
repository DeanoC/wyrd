set(LibName singleheaders)

set( CInterface
		stb_dxt.h
		stb_image.h
		stb_image_resize.h
		stb_image_write.h
		stb_truetype.h)
set( CPPInterface
		)
set( Src stb_impl.c )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "" "")
