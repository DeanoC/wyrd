#pragma once
#ifndef CORE_FREELIST_H_
#define CORE_FREELIST_H_ 1

#include <vector>
#include <atomic>

// for MTFreeList
#include "tbb/concurrent_vector.h"
#include "tbb/concurrent_queue.h"
#include <mutex>

namespace Core {

// enabled a view of free list chain in debug
#define USE_DEBUG_CRACKER

// NOT MT safe in general
// keeps freelist always big enough for all data
// TODO make this std-a-like interface 
template<typename Type, typename IndexType = uintptr_t>
class ExplicitFreeList
{
public:
	static const IndexType InvalidIndex = ~0;

	using value_type = typename std::vector<Type>::value_type;
	using size_type = typename std::vector<Type>::size_type;
	using difference_type = typename std::vector<Type>::difference_type;
	using pointer = typename std::vector<Type>::pointer;
	using const_pointer = typename std::vector<Type>::const_pointer;
	using reference = typename std::vector<Type>::reference;
	using const_reference = typename std::vector<Type>::const_reference;

	ExplicitFreeList() : currentFree( 0 )
	{}

	explicit ExplicitFreeList( size_type _count ) : data( _count ),
													freelist( _count ),
													currentFree( _count )
	{
		for(size_type i = 0; i < _count; ++i)
		{
			freelist[i] = i;
		}
	}

	IndexType push( const value_type& _val )
	{
		IndexType index = alloc();
		data[index] = _val;
		return index;
	}

	IndexType alloc()
	{
		IndexType index;
		if(currentFree == 0)
		{
			resize(IndexType((data.size() * 2) + 1));
		}
		assert( currentFree != 0 );
		index = freelist[--currentFree];
		assert( index != InvalidIndex );
		freelist[currentFree] = InvalidIndex;

		return index;
	}

	void resize( const IndexType _count )
	{
		size_type oldcount = data.size();
		assert( _count > oldcount );
		data.resize( _count );
		freelist.resize( _count );
		for(size_type i = oldcount; i < _count; ++i)
		{
			freelist[i] = InvalidIndex;
			freelist[currentFree++] = (IndexType)i;
		}
	}

	size_type size() const { return data.size(); }

	bool empty() const { return currentFree == freelist.size(); }

	void erase( IndexType const index_ )
	{
		assert( index_ < freelist.size());
		freelist[currentFree++] = index_;
	}

	Type& at( IndexType const index_ ) { return data.at(index_); }
	Type& operator[](IndexType const index_) { return data[index_]; }

	Type const& at( IndexType const index_ ) const { return data.at(index_); }
	Type const& operator[](IndexType const index_) const { return data[index_]; }

protected:
	std::vector<Type> data;
	std::vector<IndexType> freelist;
	IndexType currentFree;
};

// NOT MT safe in general
// keeps freelist always big enough for all data
// TODO make this std-a-like interface 
template<typename Type, typename IndexType = uintptr_t>
class IntrusiveFreeList
{
public:
	static_assert( sizeof( Type ) >= sizeof( IndexType ),
				   "IndexType must be less than size in bytes of the Type being stored" );
	static const IndexType InvalidIndex = ~0;

	using value_type = typename std::vector<Type>::value_type;
	using size_type = typename std::vector<Type>::size_type;
	using difference_type = typename std::vector<Type>::difference_type;
	using pointer = typename std::vector<Type>::pointer;
	using const_pointer = typename std::vector<Type>::const_pointer;
	using reference = typename std::vector<Type>::reference;
	using const_reference = typename std::vector<Type>::const_reference;

	IntrusiveFreeList() : freeCount( 0 )
	{}

	explicit IntrusiveFreeList( size_type count_ ) : data( count_ )
	{

#if defined(USE_DEBUG_CRACKER)
		debugCracker = reinterpret_cast<DebugCracker *>( &data[0] );
#endif
		// make each entry point to the next one
		for(size_type i = 0; i < count_ - 1; ++i)
		{
			IndexType *freeptr = reinterpret_cast<IndexType *>( &data[i] );
			*freeptr = i + 1;
		}

		// tail of the list
		IndexType *freeptr = reinterpret_cast<IndexType *>( &data[count_ - 1] );
		*freeptr = InvalidIndex;

		// head of the list
		freeHead = 0;
		freeCount = count_;
	}

	IndexType push(value_type const& val_ )
	{
		IndexType index = alloc();
		data[index] = val_;
		return index;
	}

	IndexType alloc()
	{
		if(freeHead == InvalidIndex)
		{
			resize((data.size() * 2) + 1 );
		}
		assert( freeHead != InvalidIndex );

		IndexType oldHead = freeHead;
		freeHead = *reinterpret_cast<IndexType *>( &data[freeHead] );
		--freeCount;
		return oldHead;
	}

	void resize( size_type const count_ )
	{
		size_type oldcount = data.size();
		assert( count_ > oldcount );
		data.resize( count_ );
#if defined(USE_DEBUG_CRACKER)
		debugCracker = reinterpret_cast<DebugCracker *>( data.data());
#endif
		for(size_type i = oldcount; i < count_ - 1; ++i)
		{
			*reinterpret_cast<IndexType *>( &data[i] ) = i + 1;
		}
		*reinterpret_cast<IndexType *>( &data[count_ - 1] ) = freeHead;
		freeHead = oldcount;
		freeCount += (count_ - oldcount);
	}

	size_type size() const { return data.size(); }

	bool empty() const { return freeCount == size(); }

	void erase( IndexType const index_ )
	{
		*reinterpret_cast<IndexType *>( &data[index_] ) = freeHead;
		freeHead = index_;
		freeCount++;
	}

	Type& at( IndexType const index_ ) { return data.at(index_); }
	Type& operator[](IndexType const index_) { return data[index_]; }

	Type const& at( IndexType const index_ ) const { return data.at(index_); }
	Type const& operator[](IndexType const index_) const { return data[index_]; }

protected:
	std::vector<Type> data;
	IndexType freeHead;
	size_type freeCount;
#if defined(USE_DEBUG_CRACKER)
	union DebugCracker
	{
		Type data;
		IndexType index;
	} *debugCracker;
#endif
};

// MTFreeList are mostly the same as FreeList but don't have resize and are MT safe
template<typename Type, typename IndexType = uintptr_t>
class MTFreeList
{
public:
	static const IndexType InvalidIndex = ~0;

	using value_type = typename tbb::concurrent_vector<Type>::value_type;
	using size_type = typename tbb::concurrent_vector<Type>::size_type;
	using difference_type = typename tbb::concurrent_vector<Type>::difference_type;
	using pointer = typename tbb::concurrent_vector<Type>*;
	using const_pointer = typename tbb::concurrent_vector<Type> const*;
	using reference = typename tbb::concurrent_vector<Type>::reference;
	using const_reference = typename tbb::concurrent_vector<Type>::const_reference;

	explicit MTFreeList(size_type _count) : data(_count)
	{
		for(size_type i = 0; i < _count; ++i)
		{
			freelist.push( (IndexType) i);
		}
	}

	IndexType push( const value_type& _val )
	{
		IndexType index = alloc();
		data[index] = _val;
		return index;
	}

	IndexType alloc()
	{
		IndexType index;
		bool space = freelist.try_pop( index );
		while(space == false)
		{
			assert(false);
		}

		return index;
	}

	bool empty() const
	{ return !freelist.empty(); }

	size_type size() const
	{ return data.size(); }

	void erase( IndexType const index_ )
	{
		freelist.push( index_ );
	}

	Type& at( IndexType const index_ )
	{ return data.at( index_ ); }

	Type& operator[]( IndexType const index_ )
	{ return data[index_]; }

	Type const& at( IndexType const index_ ) const
	{ return data.at( index_ ); }

	Type const& operator[]( IndexType const index_ ) const
	{ return data[index_]; }

protected:
	tbb::concurrent_vector<Type> data;
	tbb::concurrent_queue<IndexType> freelist;
};

template<typename Type, typename IndexType = uintptr_t, class Enable = void>
class FreeList : public ExplicitFreeList<Type, IndexType>
{
public:
	FreeList() : ExplicitFreeList<Type, IndexType>()
	{};

	explicit FreeList( typename ExplicitFreeList<Type, IndexType>::size_type count_ ) :
			ExplicitFreeList<Type, IndexType>( count_ )
	{}
};

template<typename Type, typename IndexType>
class FreeList<Type, IndexType,
		typename std::enable_if_t<
			std::is_pod<Type>{} && (sizeof(Type) >= sizeof(IndexType))
		>::value> : public IntrusiveFreeList<Type, IndexType>
{
public:
	FreeList() : IntrusiveFreeList<Type, IndexType>()
	{};

	explicit FreeList( typename IntrusiveFreeList<Type, IndexType>::size_type count_ ) :
			IntrusiveFreeList<Type, IndexType>( count_ )
	{}
};

} // end Core namespace

#endif