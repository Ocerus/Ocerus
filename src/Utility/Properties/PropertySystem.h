#ifndef _PROPERTY_SYSTEM_H
#define _PROPERTY_SYSTEM_H

#include "Property.h"
#include "../Singleton.h"

/// @name Global manager of all properties. Used to release properties.
class PropertySystem : Singleton<PropertySystem>
{
public :
	~PropertySystem();

	/// @name Returns the static list of all RTTI properties in the engine. They are released from the destructor.
	static std::list<AbstractProperty*>* GetProperties(void);
};

#endif	// _PROPERTY_SYSTEM_H