/// @file
/// Memory allocations overrides in the per-class scope.

#ifndef MemoryAlloc_h__
#define MemoryAlloc_h__

// We are currently using the standard memory allocator. Remove this when we switch to custom memory allocator.
#include <cstdlib>

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
	enum eAllocationType
	{
		ALLOC_HEAP_MAIN=0
	};

	/// @brief Custom dynamic memory allocator to allow specialized allocation of classes.
	/// @remarks The Heap template parameter specifies the heap which will be used for allocation. To make use of this
	/// allocator, simply extend the class by it.
	template<eAllocationType AllocType>
	class MemoryAlloc
	{
	public:

		explicit MemoryAlloc(void) {}
		~MemoryAlloc(void) {}

		/// Standard operator new.
		inline void* operator new(size_t sz, eCustomHeap) { return malloc(sz); }
		/// Standard operator delete.
		inline void operator delete(void* ptr, eCustomHeap) { free(ptr); }
		/// Standard operator delete.
		inline void operator delete(void* ptr) { free(ptr); }
		/// Debug operator new.
		inline void* operator new(size_t sz, eCustomHeap, const char* file, int line) { return malloc(sz); }
		/// Debug operator delete.
		inline void operator delete(void* ptr, eCustomHeap, const char* file, int line) { free(ptr); }
		/// Debug operator delete.
		inline void operator delete(void* ptr, const char* file, int line) { free(ptr); }
		/// Placement operator new.
		inline void* operator new(size_t sz, void* ptr) { return ptr; }
		/// Placement operator delete.
		inline void operator delete(void*, void*) {}
		/// Standard array operator new.
		inline void* operator new[](size_t sz, eCustomHeap) { return malloc(sz); }
		/// Standard array operator delete.
		inline void operator delete[](void* ptr, eCustomHeap) { free(ptr); }
		/// Standard array operator delete.
		inline void operator delete[](void* ptr) { free(ptr); }
		/// Debug array operator new.
		inline void* operator new[](size_t sz, eCustomHeap, const char* file, int line) { return malloc(sz); }
		/// Debug array operator delete.
		inline void operator delete[](void* ptr, eCustomHeap, const char* file, int line) { free(ptr); }
		/// Debug array operator delete.
		inline void operator delete[](void* ptr, const char* file, int line) { free(ptr); }
		/// Placement array operator new.
		inline void* operator new[](size_t sz, void* ptr) { return ptr; }
		/// Placement array operator delete.
		inline void operator delete[](void*, void*) {}

	};
}

#endif // MemoryAlloc_h__