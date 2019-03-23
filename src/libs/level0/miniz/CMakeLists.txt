set( LibName miniz)

set( CInterface
        miniz.h
        )
set( CPPInterface
        )

set( Src
        miniz.c
        )

set(Deps
        level0/core
        )


ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
