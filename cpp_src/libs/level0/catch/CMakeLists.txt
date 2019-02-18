set(LibName catch)

set(CInterface
        run.h
        )

set(CPPInterface
        catch.hpp
        )
set(Src
        )

set(Deps
        level0/core)

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}" "")