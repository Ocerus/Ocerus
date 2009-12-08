/// @file
/// Free lists / memory pooling implementation taken from Game Programming Gems 5. Edited to fit with our code and
/// comment style.

#ifndef FREELIST_H__
#define FREELIST_H__

#include "Base.h"
#include "FreeListAllocationPolicies.h"
#include "FreeListConstructionPolicies.h"
#include "FreeListGrowPolicies.h"

namespace Memory
{
	/// Optimizes allocation of objects of the same type (and size). A pool of these objects is preallocated and its
	/// items are provided when there is a need for new instance of the object. This greatly speeds things up as well as
	/// minimizes memory overhead and improves cache coherency.
	/// @remarks
	/// The behaviour of the freelist can be controlled by specified AllocationPolicy (how the freelist will allocate and
	/// free objects), ConstructionPolicy (how the freelist will construct and destruct allocated object) and
	/// GrowthPolicy (how the pool will grow when there's no room for new objects).
	template
	< 
		typename T, 
		class AllocationPolicy = Policies::LinkedListAllocation<T>,
		class ConstructionPolicy = Policies::PlacementNewConstruction<T>,
		class GrowthPolicy = Policies::DoubleGrowth<16> 
	>
	class FreeList: public AllocationPolicy, public ConstructionPolicy, public GrowthPolicy
	{
	public:

		/// Default constructor.
		FreeList();

		/// Constructs the freelist while copying internal data structures from another policy (probably present
		/// in another freelist).
		explicit FreeList( const GrowthPolicy& gp );

		/// Default destructor.
		~FreeList();

		/// Allocates a new objects from the freelist.
		T* Allocate();

		/// Frees an object previously allocated by the freelist.
		void Free( T* pObject );

		/// Allocates a new objects from the freelist while passing additional parameters to its constructor.
		template< typename A1 > T* Allocate( A1 a1 );

		/// Allocates a new objects from the freelist while passing additional parameters to its constructor.
		template< typename A1, typename A2 > T* Allocate( A1 a1, A2 a2 );

		/// Allocates a new objects from the freelist while passing additional parameters to its constructor.
		template< typename A1, typename A2, typename A3 > T* Allocate( A1 a1, A2 a2, A3 a3 );

		/// Allocates a new objects from the freelist while passing additional parameters to its constructor.
		template< typename A1, typename A2, typename A3, typename A4 > T* Allocate( A1 a1, A2 a2, A3 a3, A4 a4 );

		/// Returns the size of an item in the freelist.
		inline static size_t GetItemSize() { return sizeof(T); }

	private:

		/// Disabled.
		FreeList( const FreeList& );
		FreeList& operator=( const FreeList& );

		void* RetrieveFreeItem();
	};



	//-------------------------------------------------------------------------

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::FreeList()
	{
		uint32 numElementsToPrealloc = GrowthPolicy::GetNumberToPreallocate();
		if( numElementsToPrealloc > 0 )
			AllocationPolicy::Grow( numElementsToPrealloc );
	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::FreeList( const GrowthPolicy& gp )
		: GrowthPolicy( gp )
	{
		uint32 numElementsToPrealloc = GrowthPolicy::GetNumberToPreallocate();
		if( numElementsToPrealloc > 0 )
			AllocationPolicy::Grow( numElementsToPrealloc );
	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::~FreeList()
	{

	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	T* FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::Allocate()
	{
		void* pItem = RetrieveFreeItem();
		if( !pItem )
			return 0;
		else
			return ConstructionPolicy::Create( pItem );
	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	void FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::Free( T* pObject )
	{
		if( !pObject )
			return;

		ConstructionPolicy::Destroy( pObject );
		AllocationPolicy::Push( pObject );
	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	template< typename A1 >
	T* FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::Allocate( A1 a1 )
	{
		void* pItem = RetrieveFreeItem();
		if( !pItem )
			return 0;
		else
			return ConstructionPolicy::Create( pItem, a1 );
	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	template< typename A1, typename A2 >
	T* FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::Allocate( A1 a1, A2 a2 )
	{
		void* pItem = RetrieveFreeItem();
		if( !pItem )
			return 0;
		else
			return ConstructionPolicy::Create( pItem, a1, a2 );
	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	template< typename A1, typename A2, typename A3 >
	T* FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::Allocate( A1 a1, A2 a2, A3 a3 )
	{
		void* pItem = RetrieveFreeItem();
		if( !pItem )
			return 0;
		else
			return ConstructionPolicy::Create( pItem, a1, a2, a3 );
	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	template< typename A1, typename A2, typename A3, typename A4 >
	T* FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::Allocate( A1 a1, A2 a2, A3 a3, A4 a4 )
	{
		void* pItem = RetrieveFreeItem();
		if( !pItem )
			return 0;
		else
			return ConstructionPolicy::Create( pItem, a1, a2, a3, a4 );
	}

	template< typename T, class AllocationPolicy, class ConstructionPolicy, class GrowthPolicy >
	void* FreeList< T, AllocationPolicy, ConstructionPolicy, GrowthPolicy >::RetrieveFreeItem()
	{
		void* pItem = AllocationPolicy::Pop();
		if( !pItem )
		{
			uint32 growSize = GrowthPolicy::GetNumberToGrow( AllocationPolicy::GetNumBlocksAllocated() );
			if( growSize != 0 )
			{
				AllocationPolicy::Grow( growSize );
				pItem = AllocationPolicy::Pop();
			}
		}

		return pItem;
	}

}

#endif //FREELIST_H__