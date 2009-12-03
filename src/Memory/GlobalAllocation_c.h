/// @file
/// C api for the cusom memory allocators.

#ifndef GlobalAllocation_c_h__
#define GlobalAllocation_c_h__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/// Custom implementation of malloc.
void* CustomMalloc(size_t sz);

/// Custom implementation of free.
void CustomFree(void* ptr);


#ifdef __cplusplus
}
#endif


#endif // GlobalAllocation_c_h__