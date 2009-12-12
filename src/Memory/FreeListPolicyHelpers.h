/// @file
/// Free lists / memory pooling implementation taken from Game Programming Gems 5. Edited to fit with our code and
/// comment style.

#ifndef FreeListPolicyHelpers_h__
#define FreeListPolicyHelpers_h__

#include "BasicTypes.h"

// We need to use STL explicitely here because we have to use the default STL allocator.
#include <vector>

namespace Memory
{
	/// Helper classes and definitions for the policy classes.
	namespace PolicyHelpers
	{
		// These may need to be redefined for other platforms.  They are necessary so that
		// memory may be allocated aligned in such a manner that it can be later used by
		// classes with certain alignment requirements.
		#define AlignOf( T ) __alignof( T )
		#define AlignedMalloc( size, alignment ) CustomMalloc( size )
		#define AlignedFree( pMemory ) CustomFree( pMemory )

		/// Static Check is basically a compile time assert.  Expr is a compile time integral
		/// expression.  If it evaluates to 0 during compilation, msg, will show up in a compile error
		#define STATIC_CHECK( expr, msg ) typedef char ERROR_##msg[1][(expr)]



		/// Simple helper class each policy can use by deriving from to provide
		/// the GetNumBlocksAllocated interface necessary for Allocation Policies
		class AllocationTracker
		{
		public:

			/// Default constructor.
			AllocationTracker()
				: numBlocksAllocated(0)
				, mNumBlocksInUse(0)
				, mPeakBlocksInUse(0)
			{
			}

			/// Returns the number of memory blocks currently allocated by the freelist.
			inline uint32 GetNumBlocksAllocated() const
			{
				return numBlocksAllocated;
			}

			/// Returns the number of memory blocks currently occupied in the freelist.
			inline uint32 GetNumberOfBlocksInUse() const
			{
				return mNumBlocksInUse;
			}

			/// Returns the maximum number of memory block occupied during the life of the freelist.
			inline uint32 GetPeakNumberOfBlocksInUse() const
			{
				return mPeakBlocksInUse;
			}

		protected:

			void IncrementNumBlocksInUse()
			{
				++mNumBlocksInUse;
				if( mNumBlocksInUse > mPeakBlocksInUse )
					mPeakBlocksInUse = mNumBlocksInUse;
			}

			inline void DecrementNumBlocksInUse()
			{
				--mNumBlocksInUse;
			}

			inline void ResetBlocksInUse()
			{
				mNumBlocksInUse = 0;
			}

			unsigned int numBlocksAllocated;

		private:
			uint32 mNumBlocksInUse;
			uint32 mPeakBlocksInUse;
		};



		/// This class is used by SharedChunkPolicy, and is, itself, effectively an array of
		/// simple linked lists.  Note, that it only supports allocating objects <= maxBytes
		/// in size, and each allocation is rounded up to the nearest listSpacing size.
		/// These numbers can be easily tweaked below.
		class SharedFreeList
		{
		public:

			/// Maximum size of an object to be allocated by the freelist using this policy.
			static const int32 maxBytes = 128;

			/// Each allocation is rounded up to a multiple of this value.
			static const int32 listSpacing = 8;

			/// Number of frelists shared together.
			static const int32 numLists = maxBytes / listSpacing;

			/// Allocates a block of the given size from the internal memory storage.
			static void* Allocate( size_t size );

			/// Frees memory previously allocated by Allocate().
			static void  Deallocate( void* pBlock, size_t size );

			/// Grows the internal storage of memory blocks.
			static void  Grow( uint32 numBlocks, size_t size );

			/// Frees all allocated blocks.
			static void FreeAll();

			/// Returns the number of memory blocks currently allocated by the freelist.
			static uint32 GetNumBlocksAllocated( size_t size );

			/// Returns the number of memory blocks currently occupied in the freelist.
			static uint32 GetNumBlocksInUse( size_t size );

			/// Returns the maximum number of memory block occupied during the life of the freelist.
			static uint32 GetPeakNumberOfBlocksInUse( size_t size );

		private:

			SharedFreeList();
			~SharedFreeList();

			struct FreeList
			{
				FreeList() : pHead(0), numBlockAllocated(0),
					numBlocksInUse(0), peakBlocksInUse(0)
				{}

				void* Allocate();
				void  Deallocate( void* pBlock );

				void  PushBlock( void* pBlock );
				void  Reset();

				struct FreeBlock
				{
					FreeBlock* pNext;
				};

				FreeBlock* pHead;
				uint32 numBlockAllocated;
				uint32 numBlocksInUse;
				uint32 peakBlocksInUse;
			};

			static FreeList* GetSharedListFromSize( size_t size );
			static size_t GetAlignmentFromSize( size_t size );

			static SharedFreeList* smInstance;

			static FreeList smSharedLists[numLists];

			/// The STL container here is of purpose to make sure we use the standard STL allocator.
			static std::vector< void* > smChunks;
		};

	} // namespace PolicyHelpers
} // namespace Memory

#endif // FreeListPolicyHelpers_h__
