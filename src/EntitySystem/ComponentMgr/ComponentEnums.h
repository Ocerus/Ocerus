#ifndef _COMPONENTENUMS_H_
#define _COMPONENTENUMS_H_

namespace EntitySystem
{
	/// A set of types of all components in the system.
	enum eComponentType
	{
		CT_PLATFORM_STATS = 0,
		CT_PLATFORM_PARAMS,
		CT_PLATFORM_PHYSICS,
		CT_PLATFORM_VISUAL,
		CT_PLATFORM_LINKS,
		CT_MATERIAL,
		CT_SHIP_LOGIC,
		CT_SHIP_PHYSICS,
		CT_SHIP_VISUAL,

		NUM_COMPONENT_TYPES,
		CT_INVALID
	};
}

#endif