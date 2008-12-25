#include "Common.h"
#include "Hash.h"
#include <hash_map>

/// @name Initial seed.
#define HASH_INIT	0x811c9dc5
/// @name Prime number used for hashing.
#define HASH_PRIME	0x01000193
/// @name If set, converts between double backslashes and one backslash.
#define CONVERT_BACKSLASH
/// @name If set, it hashes case sensitive strings into different values.
#define CASE_INSENSITIVE
/// @name If set, it checks whether all hashes were unique.
#define DEBUG_STRING_HASH


uint32 HashString(const char* hashString, const int32 strLen)
{
	if (!hashString || !hashString[0])
		return INVALID_HASH;

	const unsigned char* string = (const unsigned char*)hashString;
	int32 len  = strLen==-1 ? strlen(hashString) : strLen;
	uint32 hash = HASH_INIT;

	while (len--)
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

	#ifdef DEBUG_STRING_HASH
		typedef stdext::hash_map<uint32, char*> StringHashMap;
		static StringHashMap stringMap;
		StringHashMap::const_iterator it = stringMap.find(hash);
		if (it == stringMap.end())
		{
			int32 strLen = strlen(hashString);
			char* str = DYN_NEW char[strLen+1];
			for (int32 i=0; i<strLen+1; ++i)
				str[i] = hashString[i];
			stringMap[hash] = str;
		}
		else if (strcmp(it->second, hashString))
		{
			if (LogSystem::LogMgr::GetSingletonPtr())
			{
				gLogMgr.LogMessage("HashString collision between '", it->second, "' and '", hashString, "'", LOG_ERROR);
			}
			else
			{
				const char* newString = it->second;
				const char* oldString = hashString;
				assert(!"HashString collision");
			}
		}
	#endif

	return hash;
}