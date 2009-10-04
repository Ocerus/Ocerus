/// @file
/// Platform specific settings of memory allocators.

#ifndef Memory_h__
#define Memory_h__


#include "../Memory/MemoryAlloc.h"
//#include "../Memory/GlobalAlloc.h"
using namespace Memory;

//#define new new(__FILE__, __LINE__)


/// malloc/free can't be disabled before standard libraries are included by headers following Memory.h.
/// Therefore, we can't do it here.
//#define malloc MALLOC_IS_DISABLED
//#define free FREE_IS_DISABLED


#endif // Memory_h__