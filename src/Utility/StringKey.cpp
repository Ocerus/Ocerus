#include "Common.h"
#include "StringKey.h"

//TODO predelat na pouzivani hashe

StringKey::StringKey( void )
{
	mData = 0;
}

StringKey::StringKey( const char* str )
{
	mData = HashString(str);
}

StringKey::StringKey( const String& str )
{
	mData = HashString(str.c_str());
}

StringKey::StringKey( const StringKey& rhs )
{
	mData = rhs.mData;
}

StringKey::StringKey( const char* str, const int32 numChars )
{
	mData = HashString(str, numChars);
}
StringKey& StringKey::operator=( const StringKey& rhs )
{
	mData = rhs.mData;
	return *this;
}

StringKey::~StringKey( void ) {}

bool StringKey::operator==( const StringKey& rhs ) const
{
	return mData == rhs.mData;
}

bool StringKey::operator<( const StringKey& rhs ) const
{
	return mData < rhs.mData;
}

StringKey::operator String( void ) const
{
	return DeHashString(mData);
}

StringKey::operator size_t( void ) const
{
	return mData;
}
