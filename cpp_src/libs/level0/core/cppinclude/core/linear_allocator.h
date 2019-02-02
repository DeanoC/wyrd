//! \file linear_allocator.h
//! A class for allocating RAM from a single block in
//! a simple linear fashion (i.e. simple allocs more from
//! the block, free are basically NOP)

#pragma once

#ifndef CORE_LINEAR_ALLOCATOR_H
#define CORE_LINEAR_ALLOCATOR_H

#include <cstdint>
#include "utils.h"

namespace Core
{

//! a simple linear allocator for memory doesn't use pointer (just offset)
template< int NUM_CHECKPOINTS = 8 >
class LinearAllocator
{
public:
	LinearAllocator( size_t size_ ) :
		size( size_ ),
		curFree( 0 ),
		curCheckPoint( 0 )
	{
	}

	//! reserves amt of memory from the allocator
	uintptr_t alloc( size_t amt )
	{
		uintptr_t offset = curFree;

		curFree += amt;
		assert( curFree < blockSize );
		return offset;
	}

	//! free the block, the offset must have come from Alloc
	void free( uintptr_t offset )
	{
		UNUSED( offset );
	}

	//! reset the linear allocator back to empty
	void reset()
	{
		curFree = 0;
	}

	//! get unused ram (how much is left)
	uintptr_t getUnused() const
	{
		return (m_iBlockSize - m_iCurFree);
	}

	//! A checkpoint is a known place that can be reset to
	//! leaving anything before it alone
	void pushCheckpoint()
	{
		assert( curCheckPoint < NUM_CHECKPOINTS );
		vheckPoint[ curCheckPoint++ ]  = curFree;
	}

	//! pops back to the last check point
	void popCheckpoint()
	{
		assert( curCheckPoint > 0 );
		curFree = checkPoint[ curCheckPoint-- ];
	}

	//! resets memory to a specific checkpoint, all later checkpoints and allocs are wiped
	void resetToCheckpoint( uintptr_t checkpoint )
	{
		assert( checkpoint < curCheckPoint  );
		curFree = checkPoint[ checkpoint ];
		curCheckPoint = checkpoint;
	}

protected:
	size_t const iBlockSize;
	uintptr_t curFree;
	uintptr_t checkPoint[NUM_CHECKPOINTS];
	uintptr_t curCheckPoint;
};


enum MemLinearAllocatorType
{
	MLAT_ALLOC,
	MLAT_NOALLOC,
};


template< MemLinearAllocatorType ALLOC_TYPE = MLAT_ALLOC, int NUM_CHECKPOINTS = 8 >
	class MemLinearAllocator;


template< int NUM_CHECKPOINTS >
class MemLinearAllocator<MLAT_NOALLOC,NUM_CHECKPOINTS > : protected LinearAllocator<NUM_CHECKPOINTS>
{
public:
	MemLinearAllocator( size_t sizeInBytes, uint8_t* base_ ) :
		LinearAllocator<NUM_CHECKPOINTS>( sizeInBytes ),
		base( base_ ){}

	~MemLinearAllocator() {}

	uint8_t* alloc( unsigned int amt )
	{
		return( m_pBase + LinearAllocator<NUM_CHECKPOINTS>::alloc(amt) );
	}

	void free( uint8_t* pPtr )
	{
		UNUSED( pPtr );
	}
	char* getBasePtr()
	{
		return base;
	}

	using LinearAllocator<NUM_CHECKPOINTS>::getUnused;
	using LinearAllocator<NUM_CHECKPOINTS>::reset;
	using LinearAllocator<NUM_CHECKPOINTS>::pushCheckpoint;
	using LinearAllocator<NUM_CHECKPOINTS>::popCheckpoint;
	using LinearAllocator<NUM_CHECKPOINTS>::resetToCheckpoint;
	
private:
	uint8_t* base;
};


template< int NUM_CHECKPOINTS >
class MemLinearAllocator<MLAT_ALLOC, NUM_CHECKPOINTS> : public MemLinearAllocator< MLAT_NOALLOC, NUM_CHECKPOINTS>
{
public:
	MemLinearAllocator( size_t sizeInBytes ) :
		MemLinearAllocator< MLAT_NOALLOC, NUM_CHECKPOINTS>( sizeInBytes, (uint8_t *) malloc(sizeInBytes) )
		{
		}

		~MemLinearAllocator()
		{
			free( MemLinearAllocator< MLAT_NOALLOC, NUM_CHECKPOINTS>::getBasePtr() );
		}

};


}	//namespace Core


#endif
