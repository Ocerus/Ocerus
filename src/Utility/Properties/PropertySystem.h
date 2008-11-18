//
// PropertySystem.h
//

#ifndef _PROPERTY_SYSTEM_H
#define _PROPERTY_SYSTEM_H

#include <list>
#include "Property.h"
#include "../Singleton.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PropertySystem
//
// Global manager of all properties. Used to release properties.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class PropertySystem : Singleton<PropertySystem>
{

public :

	~PropertySystem();

	static	std::list<CAbstractProperty*>*	GetProperties(void);

};

#endif	// _PROPERTY_SYSTEM_H