/// @file
/// Memory allocation overrides in the global scope.
/// @remarks
/// Note that we are not redefining the placement new/delete as it doesn't make much sense. And it causes great problems
/// as well.

#include "Common.h"
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

// We are using standard memory allocators now, so we need this include for malloc and free.
#include <cstdlib>
#undef malloc
#undef free


void* operator new( std::size_t sz ) throw(std::bad_alloc)
{
	// WARNING:
	// note that if you remove calls to the standard malloc, you have to hook your custom malloc to DbgLib's hooks
	// detecting memory leaks
	return malloc(sz);
}

void operator delete( void* ptr ) throw()
{
	// WARNING:
	// note that if you remove calls to the standard free, you have to hook your custom free to DbgLib's hooks
	// detecting memory leaks
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
