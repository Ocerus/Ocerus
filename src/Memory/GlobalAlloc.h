/// @file
/// Memory allocation overrides in the global scope.

#ifndef GlobalAlloc_h__
#define GlobalAlloc_h__

#ifdef new
#  undef new
#endif
#ifdef delete
#  undef delete
#endif


/// We are using standard memory allocators now.
#include <stdlib.h>

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

#endif // GlobalAlloc_h__