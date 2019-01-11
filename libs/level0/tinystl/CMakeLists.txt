set( Name tinystl)
set( Interface
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

set( Impl
		)

ADD_LIB(${Name} ${Interface} ${Impl} )