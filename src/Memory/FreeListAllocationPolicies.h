/// @file
/// Free lists / memory pooling implementation taken from Game Programming Gems 5. Edited to fit with our code and
/// comment style.

#ifndef FreeListAllocationPolicies_h__
#define FreeListAllocationPolicies_h__

#include "BasicTypes.h"
#include "FreeListPolicyHelpers.h"
#include "GlobalAllocation.h"

namespace Memory
{
	/// %Policies defining the behaviour of memory free lists.
	namespace Policies
	{
		/// This allocation policy is arguably the simplest, and probably the most useful and robust.
		///
		/// When a grow request is processed, an uninitialized chunk of memory is allocated
		/// to exactly hold the new blocks.  Each new uninitialized block is then cast to a FreeBlock,
		/// which is a simple link list node, and added to the link list of free blocks.
		/// This effectively hijacks the first sizeof( FreeBlock* ) of each block to point to the next free block.
		///
		/// When an object is allocated, the head block is taken from the linked list and a new T is
		/// constructed with placement new.  When an object is freed, the destructor is called and
		/// then it is cast back to a FreeBlock item and put back into the list of free blocks
		///
		/// This gives lightning fast allocation and deallocation performance while supporting objects
		/// with arbitrarily complex destructors and constructors.  It also has no per-block overhead (and only
		/// 4 bytes of overhead per chunk)
		///
		template< typename T >
		class LinkedListAllocation: public PolicyHelpers::AllocationTracker
		{
		public:

			LinkedListAllocation() : mFreeBlocks( 0 )
			{
				OC_ASSERT_MSG(sizeof(T) >= sizeof(FreeBlock*), "T must be bigger than T* to use the LinkedListAllocation free list policy");
			}

		protected:

			~LinkedListAllocation()
			{
				// free up our chunks of memory
				// NOTE:  if there are outstanding allocations, this policy won't call their destructors
				// which could lead to resource leaks
				for( std::vector< void* >::iterator it = mChunks.begin(); it != mChunks.end(); ++it )
				{
					AlignedFree( *it );
				}
			}

			void Push( void* pItem )
			{
				FreeBlock* pNewHead = reinterpret_cast< FreeBlock* > ( pItem );
				pNewHead->pNext = mFreeBlocks;
				mFreeBlocks = pNewHead;
				DecrementNumBlocksInUse();
			}

			void* Pop()
			{
				if( !mFreeBlocks )
					return 0;

				IncrementNumBlocksInUse();
				void* pReturn = mFreeBlocks;
				mFreeBlocks = mFreeBlocks->pNext;
				return pReturn;
			}

			void Grow( uint32 numBlocks )
			{
				char* pNewChunk = (char*)AlignedMalloc( numBlocks * sizeof(T), AlignOf(T) );
				numBlocksAllocated += numBlocks;

				for( uint32 ix = 0; ix < numBlocks; ++ix )
				{
					IncrementNumBlocksInUse();	// Make this call to offset the decrement inside push
					Push( pNewChunk + ix*sizeof(T) );
				}
				mChunks.push_back( pNewChunk );
			}

		private:

			struct FreeBlock
			{
				FreeBlock *pNext;
			};

			FreeBlock* mFreeBlocks;

			// The STL containers are here of a purpose to make sure we use the standard STL allocator.
			std::vector< void* > mChunks;

			// Verify at compile time that T is large enough to safely hold a FreeList*
			// NOTE, there are ways around this--pad blocks to be at least sizeof(FreeList*) or use byte offsets instead of pointers
			// Hardwire: I had to make this a run-time check since the sizeof didn't work with templates.
			//STATIC_CHECK( sizeof(T) >= sizeof(FreeBlock*), LinkedListAllocation_Requires_sizeof_T_greater_equal_sizeof_pointer );
		};



		/// This policy mimics the implementation of the freelist in Game Programming Gems 4.
		/// Notably, it constructs blocks of T by actually calling new T[], and does nothing
		/// on Create or Destroy.  This might be a benefit if the constructor call is relatively heavyweight
		/// and not strictly necessary, or possibly for freelists of POD data structures.
		///
		/// Since the objects are not reconstructed using placement new on each allocation,
		/// the trick of hijacking part of each object to link to succeeding objects can not be relied on
		/// to work since this may overwrite an object's vtable, leading to some very ugly crashes.
		/// So a separate list of free pointers is maintained as a stack.  This unfortunately adds
		/// sizof( void* ) overhead to each object allocated using this policy.
		///
		/// Because the aim of freelists is roughly to replace heap allocation using new, I strongly
		/// recommend using an implementation more similar to LinkedListAllocation unless you have good
		/// reason not to.  Especially since that policy requires no additional per-object memory overhead.
		///
		/// This is still a useful lesson to show how powerful a policy design can be.  The
		/// freelist from GPG4 can simply be thought of as one implementation in a family of freelist
		/// implementations supported by this policy based freelist.
		///
		template< typename T >
		class StackAllocation: public PolicyHelpers::AllocationTracker
		{
		private:
			struct Chunk
			{
				T* pObjects;
				uint32 numObjects;
			};

			typedef std::vector<Chunk> ChunkVector;
			typedef std::vector<void*> PointerVector;

		public:
			StackAllocation() {}

			/// The FreeAll class returns all outstanding elements to the FreeList.  This might be called at the end of
			/// a level or something.  It does not free the underlying memory.  This is the same behavior as the
			/// FreeAll function from GPG4.
			/// @remarks
			/// This is an example of a policy class exposing additional functionality to client classes. This
			/// is possible because the freelist publicly derives from the AllocationPolicy.
			void FreeAll()
			{
				mFreeObjects.clear();
				ResetBlocksInUse();

				for( typename ChunkVector::iterator it = mChunks.begin(); it != mChunks.end(); ++it )
				{
					for( unsigned int ix = 0; ix < it->numObjects; ++ix )
					{
						IncrementNumBlocksInUse();	// Make this call to offset the decrement inside push
						Push( it->pObjects + ix );
					}
				}
			}

			/// Reset frees all the memory of this allocation policy and resets it to an empty freelist.
			/// This is another demonstration of a policy class exposing additional functionality.
			void Reset()
			{
				mFreeObjects.clear();
				ResetBlocksInUse();

				for( typename ChunkVector::iterator it = mChunks.begin(); it != mChunks.end(); ++it )
				{
					AlignedFree((void*)it->pObjects);
				}

				mChunks.clear();
				numBlocksAllocated = 0;
			}

		protected:

			~StackAllocation()
			{
				Reset();
			}

			void Push( void* pItem )
			{
				DecrementNumBlocksInUse();
				mFreeObjects.push_back( pItem );
			}

			void* Pop()
			{
				if( !mFreeObjects.size() )
					return 0;
				else
				{
					IncrementNumBlocksInUse();
					void* pReturn = mFreeObjects.back();
					mFreeObjects.pop_back();
					return pReturn;
				}
			}

			void Grow( uint32 numObjects )
			{
				Chunk newChunk;
				newChunk.pObjects = (T*)AlignedMalloc( numObjects * sizeof(T), AlignOf(T) );
				newChunk.numObjects = numObjects;

				for( uint32 ix = 0; ix < newChunk.numObjects; ++ix )
				{
					IncrementNumBlocksInUse();	// Make this call to offset the decrement inside push
					Push( newChunk.pObjects + ix );
				}
				mChunks.push_back( newChunk );

				numBlocksAllocated += numObjects;
			}

		private:


			// The STL containers are here of a purpose to make sure we use the standard STL allocator.
			ChunkVector mChunks;
			PointerVector mFreeObjects;
		};



		/// The CompactableChunkAllocation is a novel free list policy that can actually reclaim
		/// unused memory in certain circumstances under certain allocation patterns.
		///
		/// The policy maintains a list of Chunks that it allocates when a request to Grow is processed.
		/// Each Chunk maintains a link list of free blocks.  When an allocation is requested, a free block
		/// from one of the chunks is returned (and constructed with placement new).  When a deallocation
		/// occurs, the block is returned to the appropriate chunk.  If the policy detects that two chunks
		/// are completely unused, then it will actually return the memory of the larger of the two chunks to the heap.
		/// This may be useful if you have many freelists in operation, which may have bursts of usage followed
		/// by relatively little usage, and it is important that every freelist is not consuming peak memory
		/// at all times.
		///
		/// Notthing is free though, and it should be noted that this policy is not as fast as the above two.
		/// Most notably, in a worst case, Free performs a linear search through all allocated chunks to find
		/// the chunk a given block was allocated from. It is likely still significantly faster than most
		/// general heap allocators.
		///
		/// This policy works best when your allocation pattern is roughly many allocations followed by many deallocations
		/// in roughly reverse order from the allocations.
		template< typename T >
		class CompactableChunkAllocation: public PolicyHelpers::AllocationTracker
		{
		public:
			CompactableChunkAllocation()
				: mAllocChunk( 0 )
				, mDeallocChunk( 0 )
			{
			}

		protected:

			~CompactableChunkAllocation()
			{
				for( typename Chunks::iterator it = mChunks.begin(); it != mChunks.end(); ++it )
				{
					it->Release();
				}
			}

			void Push( void* pItem )
			{
				OC_ASSERT(!mChunks.empty());
				OC_ASSERT(&mChunks.front() <= mDeallocChunk);
				OC_ASSERT(&mChunks.back() >= mDeallocChunk);

				mDeallocChunk  = VicinityFind(pItem);
				OC_ASSERT(mDeallocChunk);

				DoDeallocate(pItem);
				DecrementNumBlocksInUse();
			}

			void* Pop()
			{
				if( mAllocChunk == 0 || mAllocChunk->mBlocksAvailable == 0 )
				{
					// Try to find a valid chunk with an available block
					typename Chunks::iterator it = mChunks.begin();
					for(;; ++it)
					{
						if( it == mChunks.end() )
						{
							break;
						}
						if( it->mBlocksAvailable > 0 )
						{
							mAllocChunk = &*it;
							break;
						}
					}
				}

				if( mAllocChunk == 0 || mAllocChunk->mBlocksAvailable == 0 )
					return 0;
				else
				{
					IncrementNumBlocksInUse();
					return mAllocChunk->Allocate();
				}
			}

			void Grow( uint32 numBlocks )
			{
				OC_ASSERT( mAllocChunk == 0 || mAllocChunk->mBlocksAvailable == 0 );

				Chunk newChunk;
				newChunk.Init(numBlocks);
				mChunks.push_back( newChunk );
				mAllocChunk = &mChunks.front();
				mDeallocChunk = &mChunks.back();

				numBlocksAllocated += numBlocks;
			}

		private:

			struct Chunk
			{
				void Init(uint32 blocks)
				{
					OC_ASSERT( blocks > 0 );

					mData = (uint8*)AlignedMalloc( sizeof(T) * blocks, AlignOf(T) );
					OC_ASSERT( mData );

					mNumBlocks = blocks;
					mBlocksAvailable = blocks;
					mFirst = 0;

					// Link each block inside this chunk (do so in reverse order to slightly improve
					// locality of reference for elements allocated in order)
					for( int i = blocks-1; i >= 0; --i )
					{
						*((void**)(mData + i*sizeof(T))) = mFirst;
						mFirst = mData + i*sizeof(T);
					}
				}

				void* Allocate()
				{
					if( !mBlocksAvailable )
						return 0;

					OC_ASSERT( mFirst != 0 );

					void* pResult = mFirst;
					mFirst = *(void**)pResult;

					--mBlocksAvailable;
					return pResult;
				}

				void Deallocate( void* pBlock )
				{
					*(void**)pBlock = mFirst;
					mFirst = pBlock;

					++mBlocksAvailable;
					OC_ASSERT( mBlocksAvailable <= mNumBlocks );
				}

				void Release()
				{
					AlignedFree( mData );
				}

				uint8* mData;
				void* mFirst;
				uint32 mBlocksAvailable;
				uint32 mNumBlocks;
			};

			typedef std::vector< Chunk > Chunks;

			Chunks mChunks;
			Chunk* mAllocChunk;
			Chunk* mDeallocChunk;

			// This implementation of CompactableBlockPolicy has the same limiation as LinkedListAllocation
			// See Modern C++ Design for an implementation that removes this restriction (while limiting chunks
			// to contain no more than 256 blocks), as well as motivation for optimizing 1 byte allocations.
			STATIC_CHECK( sizeof(T) >= sizeof(void*), ConpactableBlockPolicy_Requires_sizeof_T_greater_equal_sizeof_pointer );

			Chunk* VicinityFind( void* p )
			{
				OC_ASSERT(!mChunks.empty());
				OC_ASSERT(mDeallocChunk);

				Chunk* lo = mDeallocChunk;
				Chunk* hi = mDeallocChunk + 1;
				Chunk* loBound = &mChunks.front();
				Chunk* hiBound = &mChunks.back() + 1;

				// Special case: deallocChunk_ is the last in the array
				if (hi == hiBound) hi = 0;

				for (;;)
				{
					if (lo)
					{
						if (p >= lo->mData && p < lo->mData + lo->mNumBlocks * sizeof(T))
						{
							return lo;
						}
						if (lo == loBound) lo = 0;
						else --lo;
					}

					if (hi)
					{
						if (p >= hi->mData && p < hi->mData + hi->mNumBlocks * sizeof(T))
						{
							return hi;
						}
						if (++hi == hiBound) hi = 0;
					}
				}

				// unreachable code
				//OC_ASSERT(false);
				//return 0;
			}

			void DoDeallocate( void* p )
			{
				OC_ASSERT(mDeallocChunk->mData <= p);
				OC_ASSERT(mDeallocChunk->mData + mDeallocChunk->mNumBlocks * sizeof(T) > p);

				// call into the chunk, will adjust the inner list but won't release memory
				mDeallocChunk->Deallocate(p);

				if (mDeallocChunk->mBlocksAvailable == mDeallocChunk->mNumBlocks)
				{
					// deallocChunk_ is completely free, should we release it?
					Chunk& lastChunk = mChunks.back();

					if (&lastChunk == mDeallocChunk)
					{
						// check if we have two last chunks empty

						if (mChunks.size() > 1 &&
							mDeallocChunk[-1].mBlocksAvailable == mDeallocChunk[-1].mNumBlocks)
						{
							// Two free chunks, discard the last one
							// Make sure we discard the one with the largest number of blocks
							if( mDeallocChunk[-1].mNumBlocks > mDeallocChunk->mNumBlocks )
							{
								std::swap( *mDeallocChunk, mDeallocChunk[-1] );
							}
							numBlocksAllocated -= lastChunk.mBlocksAvailable;
							lastChunk.Release();
							mChunks.pop_back();
							mAllocChunk = mDeallocChunk = &mChunks.front();
						}
						return;
					}

					if (lastChunk.mBlocksAvailable == lastChunk.mNumBlocks)
					{
						// Two free blocks, discard one
						// Make sure we discard the one with the largest number of blocks
						if( mDeallocChunk->mNumBlocks > lastChunk.mNumBlocks )
						{
							std::swap( *mDeallocChunk, lastChunk );
						}
						numBlocksAllocated -= lastChunk.mBlocksAvailable;
						lastChunk.Release();
						mChunks.pop_back();

						std::swap( *mDeallocChunk, mChunks.back() );
						mAllocChunk = mDeallocChunk = &mChunks.front();
					}
					else
					{
						// move the empty chunk to the end
						std::swap(*mDeallocChunk, lastChunk);
						mAllocChunk = &mChunks.back();
					}
				}
			}
		};



		/// SharedChunkAllocation is an allocation policy that actually defers its allocations to
		/// a global shared free list.
		///
		/// This means that two freelists allocating objects that are identical (or nearly identical)
		/// in size will actually share chunks and blocks of memory.  If an application uses
		/// a significant number of freelists, this can eliminate much of the excess memory stored
		/// in freelists waiting to be allocated while still giving you virtually all of the benefits of
		/// freelist allocation.  The main tradeoff you will be making by using this policy is that
		/// allocations within the same freelist are likely to be located at random areas of memory,
		/// eliminating the locality of reference benefits that individual freelists can give you.
		///
		/// It should be noted that STLPort's default allocator behaves remarkably similar to this
		/// allocation policy
		template< typename T >
		class SharedChunkAllocation
		{
		public:

			uint32 GetNumBlocksAllocated() const
			{
				return PolicyHelpers::SharedFreeList::GetNumBlocksAllocated( sizeof(T) );
			}

			uint32 GetNumberOfBlocksInUse() const
			{
				return PolicyHelpers::SharedFreeList::GetNumBlocksInUse( sizeof(T) );
			}

			uint32 GetPeakNumberOfBlocksInUse() const
			{
				return PolicyHelpers::SharedFreeList::GetPeakNumberOfBlocksInUse( sizeof(T) );
			}

		protected:

			~SharedChunkAllocation()
			{
			}

			void Push( void* pItem )
			{
				PolicyHelpers::SharedFreeList::Deallocate( pItem, sizeof(T) );
			}

			void* Pop()
			{
				return PolicyHelpers::SharedFreeList::Allocate( sizeof(T) );
			}

			void Grow( uint32 numBlocks )
			{
				PolicyHelpers::SharedFreeList::Grow( numBlocks, sizeof(T) );
			}

			// This class only works on objects that fit inside one of SharedFreeList's internal lists
			STATIC_CHECK( sizeof(T) <= (unsigned)PolicyHelpers::SharedFreeList::maxBytes,
				SharedChunkPolicy_requires_sizeof_T_not_greater_than_max_SharedFreeList_size );
		};

	}
} // namespace Memory

#endif // FreeListAllocationPolicies_h__
