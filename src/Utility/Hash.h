#ifndef Hash_h__
#define Hash_h__

#include "Settings.h"

#define INVALID_HASH 0xffffffff
#define HASH_INIT	0x811c9dc5
#define HASH_PRIME	0x01000193
#define CONVERT_BACKSLASH
#define CASE_INSENSITIVE

uint32 HashString(const char* hashString);


#endif // Hash_h__