#ifndef TextData_h__
#define TextData_h__

#include "Base.h"

namespace StringSystem
{
	/// Type of the data stored by the StringSystem.
	typedef string TextData;

	/// Container used for storing the text data.
	typedef map<StringKey, TextData> TextDataMap;
}

#endif // TextData_h__