/// @file
/// Custom hash and reverse hash functions.

#ifndef Hash_h__
#define Hash_h__

#include "Base.h"

namespace Utils
{
	/// Custom hash and reverse hash functions.
	namespace Hash
	{
		/// If hashing fails, this is what you get from the HashString function.
		const uint32 INVALID_HASH = 0xffffffff;

		/// Hashes a string into an integer value.
		/// INVALID_HASH is returned if the string cannot be hashed.
		/// @param strLen If -1 then the whole string is used.
		uint32 HashString(const char* hashString, const int32 strLen = -1);

		/// Returns one of the values previously hashed into the given hash.
		const char* DeHashString(const uint32 hash);

		/// Release all previously hashed strings. Call this when destroying application.
		void ClearHashMap();
	}
}


#endif // Hash_h__