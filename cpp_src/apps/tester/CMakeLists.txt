cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

set(TESTER_SOURCE
		tester.cpp
		catch.hpp
		level0/binify/test_binify_c.cpp
		)

add_executable(tester ${TESTER_SOURCE})
target_link_libraries(tester
		binify
		cmdlineshell
		core
		singleheaders
		lz4
		math
		os
		tinystl
		tinyformat

#		logmanager
		)
