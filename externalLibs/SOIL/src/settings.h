/// @file
/// This file is to be included in all C files of SOIL. Created by Hardwire to push custom allocator into SOIL.

#ifndef settings_h__
#define settings_h__

#include "Memory/GlobalAllocation_c.h"
#include <stdlib.h>

#define malloc(size) CustomMalloc(size)
#define free(ptr) CustomFree(ptr)


#endif // settings_h__