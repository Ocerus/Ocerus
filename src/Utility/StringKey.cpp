#include "Common.h"
#include "StringKey.h"

//TODO predelat na pouzivani hashe

StringKey::StringKey( void )
{
	mData = DYN_NEW char[1];
	mData[0] = 0;
}

StringKey::StringKey( const char* str )
{
	CreateData(str);
}

StringKey::StringKey( const string& str )
{
	CreateData(str.c_str());
}

StringKey::StringKey( const StringKey& rhs )
{
	CreateData(rhs.mData);
}

StringKey::StringKey( const char* str, const int32 numChars )
{
	CreateData(str, numChars);
}
StringKey& StringKey::operator=( const StringKey& rhs )
{
	DestroyData();
	CreateData(rhs.mData);
	return *this;
}

void StringKey::CreateData( const char* str, int32 numChars )
{
	assert(str);
	if (numChars == -1)
		numChars = strlen(str);
	mData = DYN_NEW char[numChars+1];
	for (int32 i=0; i<numChars; ++i)
		mData[i] = str[i];
	mData[numChars] = 0;
}

StringKey::~StringKey( void )
{
	DestroyData();
}

void StringKey::DestroyData( void )
{
	if (mData)
	{
		DYN_DELETE_ARRAY mData;
		mData = 0;
	}
}

bool StringKey::operator==( const StringKey& rhs ) const
{
	assert(mData);
	assert(rhs.mData);
	return strcmp(mData, rhs.mData) == 0;
}

bool StringKey::operator<( const StringKey& rhs ) const
{
	assert(mData);
	assert(rhs.mData);
	return strcmp(mData, rhs.mData) < 0 ;
}

StringKey::operator string( void ) const
{
	return string(mData);
}

StringKey::operator size_t( void ) const
{
	return HashString(mData);
}
