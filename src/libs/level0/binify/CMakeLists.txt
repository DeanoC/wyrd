set( LibName binify)
set(CMAKE_CXX_STANDARD 11)

set( CInterface
		binify_c.h
		)
set( CPPInterface
		)
set( Src
		binify.cpp
		binify.h
		FlexLexer.h
		location.hh
		parser.cxx
		parser.hxx
		position.hh
		scanner.cxx
		scanner.h
		scanner.hxx
		stack.hh
		)
set( Deps
        level0/core
        )

set(Tests
        test_binify.cpp
        test_binify_c.cpp
        )

ADD_LIB(${LibName} "${CInterface}" "${CPPInterface}" "${Src}" "${Deps}")
ADD_LIB_TESTS(${LibName} "${CInterface}" "${CPPInterface}" "${Tests}")
