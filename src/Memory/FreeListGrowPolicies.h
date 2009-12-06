/// @file
/// Free lists / memory pooling implementation taken from Game Programming Gems 5. Edited to fit with our code and
/// comment style.

#ifndef FreeListGrowPolicies_h__
#define FreeListGrowPolicies_h__

#include "Base.h"
#include "FreeListPolicyHelpers.h"

namespace Memory
{
	namespace Policies
	{
		/// The freelist will growth always by the same number blocks. A maximum number of blocks must be specified as well
		/// to prevent unwanted freelist overgrowths.
		template< int32 NumToPrealloc, int32 NumToGrow, int32 MaxAllocated >
		struct ConstantGrowthPolicy
		{
			inline static uint32 GetNumberToPreallocate()
			{
				return NumToPrealloc;
			}

			static uint32 GetNumberToGrow( uint32 numAllocated )
			{
				if( numAllocated + NumToGrow > MaxAllocated )
					return 0;
				else
					return NumToGrow;
			}
		};



		/// The freelist will growth always by the same number blocks. The number is determined from the desired size of a block
		/// and the size of a freelist item. The number will always be at least 8.
		template< typename T, int32 BlockSize >
		struct ConstantChunkSizeGrowthPolicy
		{
			static const int32 numberToGrow = (BlockSize / sizeof(T) > 8) ? (BlockSize / sizeof(T)) : 8;

			inline static uint32 GetNumberToPreallocate()
			{
				return 0;
			}

			inline static uint32 GetNumberToGrow( uint32 )
			{
				return numberToGrow;
			}
		};


		/// The freelist will growth the twice its size. An initial size must be supplied.
		template< int InitialSize > struct DoubleGrowthSize
		{
			static unsigned int GetNumberToPreallocate()
			{
				return 0;
			}

			static unsigned int GetNumberToGrow( unsigned int numAllocated )
			{
				if( numAllocated == 0 )
					return InitialSize;
				else
					return numAllocated;
			}
		};

	}
} // namespace Memory

#endif // FreeListGrowPolicies_h__
