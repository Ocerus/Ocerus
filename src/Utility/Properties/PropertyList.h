#ifndef PropertyList_h__
#define PropertyList_h__

#include "PropertyHolder.h"
#include "../StringKey.h"

/// @name This trick had to be done to avoid circular include dependencies (I can forward declare this class).
class PropertyList : public Map<StringKey, PropertyHolder>
{

};


#endif // PropertyList_h__