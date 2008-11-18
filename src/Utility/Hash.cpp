#include "Common.h"
#include "Hash.h"

uint32 HashString(const char* hashString)
{
	if (!hashString || !hashString[0])
		return INVALID_HASH;

	const unsigned char* string = (const unsigned char*)hashString;
	uint32 hash = HASH_INIT;

	while (*string)
	{
		hash *= HASH_PRIME;

		char c = *string++;

#ifdef CONVERT_BACKSLASH
		if (c == '\\')
		{
			c = '/';
		}
#endif

#ifdef CASE_INSENSITIVE
		if ((c >= 'a') && (c <= 'z'))
		{
			c -= 'a' - 'A';
		}
#endif
		hash ^= (uint32)c;
	}
	return hash;
}