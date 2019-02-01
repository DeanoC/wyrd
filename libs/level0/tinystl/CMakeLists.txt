set( Name tinystl )
set( CInterface
		)
set( CPPInterface
		allocator.h
		buffer.h
		hash.h
		hash_base.h
		new.h
		stddef.h
		string.h
		string_view.h
		traits.h
		unordered_map.h
		unordered_set.h
		vector.h
		)
set( Src
		)

ADD_LIB(${Name} "${CInterface}" "${CPPInterface}" "${Src}" "" )