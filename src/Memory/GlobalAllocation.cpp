/// @file
/// Memory allocation overrides in the global scope.
/// @remarks
/// Note that we are not redefining the placement new/delete as it doesn't make much sense. And it causes great problems
/// as well.

#include "Common.h"
#include "GlobalAllocation.h"
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


void* operator new( std::size_t sz ) throw(std::bad_alloc)
{
	return Memory::CustomMalloc(sz);
}

void operator delete( void* ptr ) throw()
{
	Memory::CustomFree(ptr);
}

void* operator new( std::size_t sz, const std::nothrow_t& ) throw()
{
	return Memory::CustomMalloc(sz);
}

void operator delete( void* ptr, const std::nothrow_t& ) throw()
{
	Memory::CustomFree(ptr);
}

void* operator new[]( std::size_t sz ) throw(std::bad_alloc)
{
	return Memory::CustomMalloc(sz);
}

void operator delete[]( void* ptr ) throw()
{
	Memory::CustomFree(ptr);
}

void* operator new[]( std::size_t sz, const std::nothrow_t& ) throw()
{
	return Memory::CustomMalloc(sz);
}

void operator delete[]( void* ptr, const std::nothrow_t& ) throw()
{
	Memory::CustomFree(ptr);
}


#include <angelscript.h>

void Memory::InitGlobalMemoryAllocation( void )
{
	AngelScript::asSetGlobalMemoryFunctions(CustomMalloc, CustomFree);
}


#include "GlobalAllocation_c.h"

void* CustomMalloc(size_t sz)
{
	return Memory::CustomMalloc(sz);
}

void CustomFree(void* ptr)
{
	Memory::CustomFree(ptr);
}
