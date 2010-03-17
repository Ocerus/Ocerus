#include "Common.h"
#include "StringKey.h"
#include <iostream>

const StringKey StringKey::Null;

struct StringKeyMap: public hash_map<string, StringKey::StringKeyData*>
{
    StringKeyMap()
	{
		OC_ASSERT(smInstance == 0);
		smInstance = this;
	}

	~StringKeyMap()
	{
		OC_ASSERT(smInstance != 0);
		smInstance = 0;
	}

	static StringKeyMap* GetInstance()
	{
		static StringKeyMap instance;
		return smInstance;
	}

	static StringKeyMap* smInstance;
};

StringKeyMap* StringKeyMap::smInstance = 0;

inline hash_map<string, StringKey::StringKeyData*>* GetStringKeyMap()
{
	static hash_map<string, StringKey::StringKeyData*>* stringKeyMap;
	return stringKeyMap;
}

inline StringKey::StringKeyData* GetStringKeyData(const string& str)
{
	StringKey::StringKeyData* res = 0;
	StringKeyMap* stringKeyMap = StringKeyMap::GetInstance();
	if (stringKeyMap == 0)
	{
		return 0;
	}
	else if (stringKeyMap->find(str) == stringKeyMap->end())
	{
		res = new StringKey::StringKeyData(str);
		(*stringKeyMap)[str] = res;
	}
	else
	{
		res = (*stringKeyMap)[str];
	}
	return res;
}

StringKey::StringKey( void )
{
	mData = 0;
}

StringKey::StringKey(const char* str): mData(GetStringKeyData(str))
{
	RefCntInc();
}

StringKey::StringKey(const char* str, size_t length): mData(GetStringKeyData(string(str, length)))
{
	RefCntInc();
}

StringKey::StringKey( const string& str ): mData(GetStringKeyData(str))
{
	RefCntInc();
}

StringKey::StringKey( const StringKey& rhs )
{
	mData = rhs.mData;
	RefCntInc();
}

StringKey& StringKey::operator=( const StringKey& rhs )
{
	if (this == &rhs) { return *this; }
	RefCntDec();
	mData = rhs.mData;
	RefCntInc();
	return *this;
}

StringKey::~StringKey( void )
{
	RefCntDec();
}

string& StringKey::ToString( void ) const
{
	static string nullStringKey("NULL StringKey");
	return mData ? mData->mRefString : nullStringKey;
}

StringKey::operator size_t( void ) const
{
	return (size_t)mData;
}

void StringKey::RefCntInc()
{
	if (mData)
	{
		mData->mRefCount++;
	}
}

void StringKey::RefCntDec()
{
	if (mData && --mData->mRefCount == 0)
	{
		StringKeyMap* stringKeyMap = StringKeyMap::GetInstance();
		if (stringKeyMap)
			stringKeyMap->erase(mData->mRefString);
		delete mData;
	}
}
