#ifndef _COMPONENTENUMS_H_
#define _COMPONENTENUMS_H_

namespace EntitySystem
{
	/// A set of types of all components in the system.
	enum eComponentType
	{
		CT_PLATFORM = 0,

		NUM_COMPONENT_TYPES,
		CT_INVALID
	};
}

#endif