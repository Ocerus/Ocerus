/// @file
/// Memory allocations overrides and settings in the global scope.

#ifndef GlobalAllocation_h__
#define GlobalAllocation_h__

// We are currently using the standard memory allocator. Remove this when we switch to custom memory allocator.
#include <cstdlib>

namespace Memory
{
	/// Initializes global memory allocation structures.
	void InitGlobalMemoryAllocation(void);

	/// Custom implementation of malloc.
	inline void* CustomMalloc(std::size_t sz)
	{
		// WARNING:
		// note that if you remove calls to the standard malloc, you have to hook your custom malloc to DbgLib's hooks
		// detecting memory leaks
		return malloc(sz);
	}

	/// Custom implementation of free.
	inline void CustomFree(void* ptr)
	{
		// WARNING:
		// note that if you remove calls to the standard free, you have to hook your custom free to DbgLib's hooks
		// detecting memory leaks
		free(ptr);
	}

}

#endif // GlobalAllocation_h__