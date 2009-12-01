/// @file
/// Memory allocations overrides in the per-class scope.

#ifndef ClassAllocation_h__
#define ClassAllocation_h__

#include <exception>
#include <new>

#ifdef new
#  undef new
#endif
#ifdef delete
#  undef delete
#endif

#ifdef __WIN__
// Supress the warning of unused throw reference.
#pragma warning(disable: 4290)
#endif

#include "GlobalAllocation.h"

/// Custom memory management.
namespace Memory
{
	/// Different heaps available for allocation.
	enum eAllocationType
	{
		ALLOC_HEAP_MAIN=0
	};

	/// Custom dynamic memory allocator to allow specialized allocation of classes.
	/// The AllocType template parameter specifies the heap which will be used for allocation. To make use of this
	/// allocator, simply extend the class by it.
	template<eAllocationType AllocType>
	class ClassAllocation
	{
	public:

		explicit ClassAllocation(void) {}
		~ClassAllocation(void) {}

		inline void* operator new( std::size_t sz ) throw(std::bad_alloc)
		{
			return CustomMalloc(sz);
		}

		inline void operator delete( void* ptr ) throw()
		{
			CustomFree(ptr);
		}

		inline void* operator new( std::size_t sz, const std::nothrow_t& )
		{
			return CustomMalloc(sz);
		}

		inline void operator delete( void* ptr, const std::nothrow_t& )
		{
			CustomFree(ptr);
		}

		inline void* operator new[]( std::size_t sz ) throw(std::bad_alloc)
		{
			return CustomMalloc(sz);
		}

		inline void operator delete[]( void* ptr ) throw()
		{
			CustomFree(ptr);
		}

		inline void* operator new[]( std::size_t sz, const std::nothrow_t& )
		{
			return CustomMalloc(sz);
		}

		inline void operator delete[]( void* ptr, const std::nothrow_t& )
		{
			CustomFree(ptr);
		}
	};
}

#endif // ClassAllocation_h__