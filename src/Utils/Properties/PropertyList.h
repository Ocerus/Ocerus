/// @file
/// A list for storing properties in their PropertyHolder.

#ifndef PropertyList_h__
#define PropertyList_h__

#include "Base.h"

namespace Reflection
{
	/// A list for storing properties in their PropertyHolder.
	/// This trick had to be done to avoid circular include dependencies (I can forward declare this class, 
	/// but not the typedef).
	class PropertyList : public list<PropertyHolder>
	{

	};
}


#endif // PropertyList_h__