#ifndef PropertyList_h__
#define PropertyList_h__

#include "PropertyHolder.h"
#include "../StringKey.h"
#include <map>

/// @name This trick had to be done to avoid circular include dependencies (I can forward declare this class).
class PropertyList : public std::map<StringKey, PropertyHolder>
{

};


#endif // PropertyList_h__