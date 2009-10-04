/// @file
/// Platform specific settings of memory allocators.
/// @remarks
/// This file is included as a first include file in the whoe project.

#ifndef Memory_h__
#define Memory_h__


/// Using this type we differentiate our custom allocation operators from the standard ones.
enum eCustomHeap { CUSTOM_HEAP };


#include "../Memory/MemoryAlloc.h"
#include "../Memory/GlobalAlloc.h"
using namespace Memory;


#endif // Memory_h__