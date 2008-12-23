#ifndef PropertyList_h__
#define PropertyList_h__

#include "PropertyHolder.h"

/// @name This trick had to be done to avoid circular include dependencies (I can forward declare this class).
class PropertyList : public std::map<string, PropertyHolder>
{

};


#endif // PropertyList_h__