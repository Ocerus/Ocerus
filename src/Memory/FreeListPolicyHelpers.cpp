#include "Common.h"
#include "FreeListPolicyHelpers.h"

namespace Memory
{
	namespace PolicyHelpers
	{
		SharedFreeList* SharedFreeList::smInstance = 0;
		SharedFreeList::FreeList SharedFreeList::smSharedLists[SharedFreeList::numLists];
		std::vector< void* > SharedFreeList::smChunks;

		void* SharedFreeList::FreeList::Allocate()
		{
			if( !pHead )
				return 0;

			++numBlocksInUse;
			if( numBlocksInUse > peakBlocksInUse )
				peakBlocksInUse = numBlocksInUse;

			void* pReturn = pHead;
			pHead = pHead->pNext;
			return pReturn;
		}

		void SharedFreeList::FreeList::Deallocate( void* pBlock )
		{
			--numBlocksInUse;
			PushBlock( pBlock );
		}

		void SharedFreeList::FreeList::PushBlock( void* pBlock )
		{
			FreeBlock* pNewHead = reinterpret_cast< FreeBlock* >( pBlock );
			pNewHead->pNext = pHead;
			pHead = pNewHead;
		}

		void SharedFreeList::FreeList::Reset()
		{
			pHead = 0;
			numBlocksInUse = 0;
			peakBlocksInUse = 0;
			numBlockAllocated = 0;
		}

		SharedFreeList::FreeList* SharedFreeList::GetSharedListFromSize( size_t size )
		{
			OC_ASSERT( size != 0 );
			OC_ASSERT( size < (size_t)maxBytes );

			unsigned int index = (size-1) / listSpacing;
			OC_ASSERT( index < (unsigned int)SharedFreeList::numLists );

			return smSharedLists + index;
		}

		size_t SharedFreeList::GetAlignmentFromSize( size_t size )
		{
			OC_ASSERT( size != 0 );

			// Round up the size to the nearest listSpacing
			size_t roundedSize = ( size + (listSpacing+1) ) & (~(listSpacing-1));

			// define minimum and maximum alignment (64 as max alignment is somewhat arbitrary)
			const size_t minAlign = listSpacing;
			const size_t maxAlign = (listSpacing > 64) ? listSpacing : 64;

			for( size_t align = minAlign; align < maxAlign; ++align )
			{
				if( roundedSize & align )
					return (align < minAlign) ? minAlign : align;
			}

			return maxAlign;
		}

		void* SharedFreeList::Allocate( size_t size )
		{
			FreeList* pSharedList = GetSharedListFromSize( size );
			return pSharedList->Allocate();
		}

		void SharedFreeList::Deallocate( void* pBlock, size_t size )
		{
			FreeList* pSharedList = GetSharedListFromSize( size );
			pSharedList->Deallocate( pBlock );
		}

		void SharedFreeList::Grow( uint32 numBlocks, size_t size )
		{
			FreeList* pFreeList = GetSharedListFromSize( size );

			// Note that to be safe, we must align the chunk allocations to the nearest power
			// of two for the given list size.  I.e. 16 byte classes may or may not need to be
			// aligned to 16 bytes, so to be safe, all chunks capable of handling 16 byte classes
			// must be aligned to 16 bytes.  Similarly 24 byte chunks must be aligned
			// to 8 bytes, etc. etc.
			char* pChunk = (char*)AlignedMalloc( numBlocks*size, GetAlignmentFromSize(size) );

			for( uint32 ix = 0; ix < numBlocks; ++ix )
			{
				pFreeList->PushBlock( pChunk+ ix*size );
			}

			pFreeList->numBlockAllocated += numBlocks;
			smChunks.push_back( pChunk );
		}

		void SharedFreeList::FreeAll()
		{
			// Free all the memory
			for( std::vector< void* >::iterator it = smChunks.begin(); it != smChunks.end(); ++it )
			{
				AlignedFree( *it );
			}

			// Empty all the free lists
			for( int ix = 0; ix < numLists; ++ix )
				smSharedLists[ix].Reset();
		}

		uint32 SharedFreeList::GetNumBlocksAllocated( size_t size )
		{
			return GetSharedListFromSize( size )->numBlockAllocated;
		}

		uint32 SharedFreeList::GetNumBlocksInUse( size_t size )
		{
			return GetSharedListFromSize( size )->numBlocksInUse;
		}

		uint32 SharedFreeList::GetPeakNumberOfBlocksInUse( size_t size )
		{
			return GetSharedListFromSize( size )->peakBlocksInUse;
		}
	}
}
