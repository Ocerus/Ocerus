#include "Common.h"
#include "Hash.h"

/// Initial seed used for hashing.
#define HASH_INIT	0x811c9dc5
/// Prime number used for hashing.
#define HASH_PRIME	0x01000193
/// If set, converts between double backslashes and one backslash.
#define CONVERT_BACKSLASH
/// If set, it hashes case sensitive strings into different values.
#define CASE_INSENSITIVE
/// If set, it checks whether all hashes were unique.
#define DEBUG_STRING_HASH


#ifdef DEBUG_STRING_HASH 

typedef stdext::hash_map<uint32, char*> StringHashMap;

static StringHashMap& GetStringHashMap(void)
{
	static StringHashMap stringMap;
	return stringMap;
}

#endif

uint32 Hash::HashString(const char* hashString, const int32 strLen)
{
	if (!hashString || !hashString[0])
		return Hash::INVALID_HASH;

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
		len  = strLen==-1 ? strlen(hashString) : strLen;
		StringHashMap& stringMap = GetStringHashMap();
		StringHashMap::const_iterator it = stringMap.find(hash);
		if (it == stringMap.end())
		{
			char* str = DYN_NEW char[len+1];
			for (int32 i=0; i<len; ++i)
				str[i] = hashString[i];
			str[len] = 0;
			stringMap[hash] = str;
		}
		else if (strncmp(it->second, hashString, len))
		{
			if (LogSystem::LogMgr::GetSingletonPtr())
			{
				gLogMgr.LogMessage("HashString collision between '", it->second, "' and '", hashString, "'", LOG_ERROR);
			}
			else
			{
				const char* newString = it->second;
				const char* oldString = hashString;
				BS_ASSERT(!"HashString collision");
			}
		}
	#endif

	return hash;
}

const char* Hash::DeHashString( const uint32 hash )
{
	StringHashMap& stringMap = GetStringHashMap();
	StringHashMap::const_iterator it = stringMap.find(hash);
	if (it == stringMap.end())
		return "DeHashString_FAILED";
	return it->second;
}