/// @file
/// Memory allocations overrides in the per-class scope.

#ifndef MemoryAlloc_h__
#define MemoryAlloc_h__

// We are currently using the standard memory allocator. Remove this when we switch to custom memory allocator.
#include <stdlib.h>

#ifdef new
#  undef new
#endif
#ifdef delete
#  undef delete
#endif

/// Custom memory management.
namespace Memory
{
	/// Different heaps available for allocation.
	enum eHeap
	{
		HEAP_MAIN=0
	};

	/// @brief Custom dynamic memory allocator.
	/// @remarks The Heap template parameter specifies the heap which will be used for allocation.
	//template<eHeap Heap = HEAP_MAIN>
	class MemoryAlloc
	{
	public:

		explicit MemoryAlloc(void) {
			int a=0;
		}
		~MemoryAlloc(void) {}

		/// Operator new, with debug line info.
		void* operator new(size_t sz, const char* file, int line)
		{
			return malloc(sz);
		}

		/// Standard operator new.
		void* operator new(size_t sz)
		{
			return malloc(sz);
		}

		/// Placement operator new.
		void* operator new(size_t sz, void* ptr)
		{
			return ptr;
		}

		/// Array operator new, with debug line info.
		void* operator new[] ( size_t sz, const char* file, int line )
		{
			return malloc(sz);
		}

		/// Standard array operator new.
		void* operator new[] ( size_t sz )
		{
			return malloc(sz);
		}

		/// Standard operator delete.
		void operator delete( void* ptr )
		{
			free(ptr);
		}

		/// @brief Operator delete with debug info.
		/// @remarks Only called if there is an exception in corresponding 'new'.
		void operator delete( void* ptr, const char* , int )
		{
			free(ptr);
		}

		/// Standard array operator delete.
		void operator delete[] ( void* ptr )
		{
			free(ptr);
		}

		/// Array operator delete with debug info.
		void operator delete[] ( void* ptr, const char* , int )
		{
			free(ptr);
		}		

	};
}

#endif // MemoryAlloc_h__