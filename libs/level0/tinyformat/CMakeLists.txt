set(LibName tinyformat)

set( Interface tinyformat.h)
set( Src tinyformat.cpp )

ADD_LIB(${LibName} ${Interface} ${Src})

target_compile_definitions(
		${LibName}
		INTERFACE
		TINYFORMAT_IMPLEMENTATION_FILE=0)