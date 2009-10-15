#include "Common.h"
#include "Hash.h"

/// Initial seed used for hashing.
#define HASH_INIT	0x811c9dc5
/// Prime number used for hashing.
#define HASH_PRIME	0x01000193
/// If set, converts backslash to slash.
#define CONVERT_BACKSLASH
/// If set, it hashes and store strings case insensitive.
#define CASE_INSENSITIVE

typedef hash_map<uint32, char*> StringHashMap;

static StringHashMap& GetStringHashMap(void)
{
	static StringHashMap stringMap;
	return stringMap;
}

uint32 Hash::HashString(const char* hashString, const int32 strLen)
{
	if (!hashString || !hashString[0])
		return Hash::INVALID_HASH;

	// convert input to nonconst null-terminated string,
	// hash computation and string conversion
	int32 len  = strLen==-1 ? strlen(hashString) : strLen;
	uint32 hash = HASH_INIT;
	char* str = new char[len+1]; 
	for (int32 i=0; i<len; ++i)
	{
		hash *= HASH_PRIME;
		str[i] = hashString[i];
		#ifdef CONVERT_BACKSLASH
			if (str[i] == '\\') str[i] = '/';
		#endif
		#ifdef CASE_INSENSITIVE
			str[i] = (char)toupper(str[i]);
		#endif
	    hash ^= (uint32)(str[i]);
	}
	str[len] = 0;
	
	// save hash and string to StringHashMap and resolve collision
	StringHashMap& stringMap = GetStringHashMap();
	StringHashMap::const_iterator it = stringMap.find(hash);
	if (it == stringMap.end())
	{
		stringMap[hash] = str;
	}
	else 
	{
		while (strncmp(it->second, str, len+1)) // collision
		{
			if ((it = stringMap.find(++hash)) == stringMap.end())
			{
				stringMap[hash] = str;
				return hash;
			}
		}
		delete[] str; // in stringMap is already same string as str
	}
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

void Hash::ClearHashMap()
{
	StringHashMap& stringMap = GetStringHashMap();
	for (StringHashMap::const_iterator it = stringMap.begin(); it!= stringMap.end(); ++it)
	{
		delete[] (*it);
	}
	stringMap.clear();
}
