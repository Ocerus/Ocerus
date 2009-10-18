/// @file
/// Memory allocations overrides in the per-class scope.

#ifndef MemoryAlloc_h__
#define MemoryAlloc_h__

#include <exception>
#include <new>

#ifdef new
#  undef new
#endif
#ifdef delete
#  undef delete
#endif

// Supress the warning of unused throw reference.
#pragma warning(disable: 4290)


// We are currently using the standard memory allocator. Remove this when we switch to custom memory allocator.
#include <cstdlib>


/// Custom memory management.
namespace Memory
{
	/// Different heaps available for allocation.
	enum eAllocationType
	{
		ALLOC_HEAP_MAIN=0
	};

	/// Custom dynamic memory allocator to allow specialized allocation of classes.
	/// The Heap template parameter specifies the heap which will be used for allocation. To make use of this
	/// allocator, simply extend the class by it.
	template<eAllocationType AllocType>
	class MemoryAlloc
	{
	public:

		explicit MemoryAlloc(void) {}
		~MemoryAlloc(void) {}

		void* operator new( std::size_t sz ) throw(std::bad_alloc)
		{
			return malloc(sz); 
		}

		void operator delete( void* ptr ) throw()
		{
			free(ptr);
		}

		void* operator new( std::size_t sz, const std::nothrow_t& )
		{
			return malloc(sz); 
		}

		void operator delete( void* ptr, const std::nothrow_t& )
		{
			free(ptr);
		}

		void* operator new[]( std::size_t sz ) throw(std::bad_alloc)
		{
			return malloc(sz);
		}

		void operator delete[]( void* ptr ) throw()
		{
			free(ptr);
		}

		void* operator new[]( std::size_t sz, const std::nothrow_t& )
		{
			return malloc(sz);
		}

		void operator delete[]( void* ptr, const std::nothrow_t& )
		{
			free(ptr);
		}
	};
}

#endif // MemoryAlloc_h__