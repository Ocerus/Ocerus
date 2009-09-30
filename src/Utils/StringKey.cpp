#include "Common.h"
#include "StringKey.h"
#include "Hash.h"

//TODO predelat na pouzivani hashe

StringKey::StringKey( void )
{
	mData = 0;
}

StringKey::StringKey( const char* str )
{
	mData = Hash::HashString(str);
}

StringKey::StringKey( const string& str )
{
	mData = Hash::HashString(str.c_str());
}

StringKey::StringKey( const StringKey& rhs )
{
	mData = rhs.mData;
}

StringKey::StringKey( const char* str, const int32 numChars )
{
	mData = Hash::HashString(str, numChars);
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

StringKey::operator string( void ) const
{
	return Hash::DeHashString(mData);
}

StringKey::operator size_t( void ) const
{
	return mData;
}
