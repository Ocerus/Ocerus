#ifndef StringKey_h__
#define StringKey_h__

#include "Settings.h"

/// @name This class serves as a key into maps and other structures where we want to index using string, but se need speed.
class StringKey
{
public:
	/// @name Multiple constructors.
	//@{
	StringKey(void);
	StringKey(const char* str);
	StringKey(const char* str, const int32 numChars);
	StringKey(const string& str);
	StringKey(const StringKey& rhs);
	//@}

	~StringKey(void);

	/// @name Assignment operator.
	StringKey& operator=(const StringKey& rhs);
	
	/// @name Conversion operators.
	//@{
	operator string (void) const;
	operator size_t (void) const;
	//@}

	/// @name Comparison operators.
	//@{
	bool operator==(const StringKey& rhs) const;
	bool operator<(const StringKey& rhs) const;
	//@}

private:
	char* mData;

	void CreateData(const char* str, int32 numChars = -1);
	void DestroyData(void);
};

#endif // StringKey_h__