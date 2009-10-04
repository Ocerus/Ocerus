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


#define __PLACEMENT_NEW_INLINE
#define __PLACEMENT_VEC_NEW_INLINE
#include <new>

/// We are using standard memory allocators now, so we need this include for malloc and free.
#include <cstdlib>


/// Standard operator new.
inline void* operator new(size_t sz, eCustomHeap) { return malloc(sz); }
/// Standard operator delete.
inline void operator delete(void* ptr, eCustomHeap) { free(ptr); }
/// Debug operator new.
inline void* operator new(size_t sz, eCustomHeap, const char* file, int line) { return malloc(sz); }
/// Debug operator delete.
inline void operator delete(void* ptr, eCustomHeap, const char* file, int line) { free(ptr); }
/// Placement operator new.
inline void* operator new(size_t sz, void* ptr) { return ptr; }
/// Placement operator delete.
inline void operator delete(void*, void*) {}
/// Standard array operator new.
inline void* operator new[](size_t sz, eCustomHeap) { return malloc(sz); }
/// Standard array operator delete.
inline void operator delete[](void* ptr, eCustomHeap) { free(ptr); }
/// Debug array operator new.
inline void* operator new[](size_t sz, eCustomHeap, const char* file, int line) { return malloc(sz); }
/// Debug array operator delete.
inline void operator delete[](void* ptr, eCustomHeap, const char* file, int line) { free(ptr); }
/// Placement array operator new.
inline void* operator new[](size_t sz, void* ptr) { return ptr; }
/// Placement array operator delete.
inline void operator delete[](void*, void*) {}


#endif // GlobalAlloc_h__