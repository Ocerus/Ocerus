#ifndef _COMPONENTENUMS_H_
#define _COMPONENTENUMS_H_

namespace EntitySystem
{
	/// @name A set of types of all components in the system.
	enum eComponentType
	{
		#define COMPONENT_TYPE(x, y) x,
		#include "ComponentTypes.h"
		#undef COMPONENT_TYPE

		NUM_COMPONENT_TYPES,
		CT_INVALID
	};

	/// @name String names of all component types.
	const char* const ComponentTypeNames[] =
	{
		#define COMPONENT_TYPE(x, y) #x,
		#include "ComponentTypes.h"
		#undef COMPONENT_TYPE
	};

	/// @name Returns a component type based on a string.
	eComponentType DetectComponentType(const String& type);

}

#endif