/// @file
/// Definition of the data StringSystem works with.

#ifndef TextData_h__
#define TextData_h__

#include "Base.h"
#include "CEGUIString.h"

namespace StringSystem
{
	/// Type of the data stored by the StringSystem.
	typedef CEGUI::String TextData;

	/// Container used for storing the text data.
	typedef map<StringKey, TextData> TextDataMap;
	
	/// Container used for storing the text data in various groups.
	typedef map<StringKey, TextDataMap> GroupTextDataMap;
}

#endif // TextData_h__