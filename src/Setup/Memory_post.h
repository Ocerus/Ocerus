/// @file
/// Platform specific settings of memory allocators.
/// @remarks
/// This file is included after all standard libraries were included.

#ifndef Memory_post_h__
#define Memory_post_h__

#include "Memory_pre.h"


/// We are disabling malloc and free as it's an old C-style way of allocating memory.
/// We must define these macros here to prevent conflicts with standard libraries included before this line.
#define malloc MALLOC_IS_DISABLED
#define free FREE_IS_DISABLED


#endif // Memory_post_h__