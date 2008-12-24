#ifndef Hash_h__
#define Hash_h__

#include "Settings.h"

/// @name If hashing fails, this is what you get from the HashString function.
#define INVALID_HASH 0xffffffff

/// @name Hashes a string into an integer value.
uint32 HashString(const char* hashString);


#endif // Hash_h__