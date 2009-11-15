#include "Common.h"
#include "StringKey.h"
#include <iostream>

const StringKey StringKey::Null;

inline hash_map<string, StringKey::StringKeyData*>& GetStringKeyMap()
{
	static hash_map<string, StringKey::StringKeyData*> stringKeyMap;
	return stringKeyMap;
}

inline StringKey::StringKeyData* GetStringKeyData(const string& str)
{
	StringKey::StringKeyData* res = 0;
	if (GetStringKeyMap().find(str) == GetStringKeyMap().end())
	{
		res = new StringKey::StringKeyData(str);
		GetStringKeyMap()[str] = res;
	}
	else
	{
		res = GetStringKeyMap()[str];
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

string StringKey::ToString( void ) const
{
	return mData ? mData->mRefString : "NULL StringKey";
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
		GetStringKeyMap().erase(mData->mRefString);
		delete mData;
	}
}
