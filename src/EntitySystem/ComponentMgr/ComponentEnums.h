/// @file
/// Definitions of all entity components to be registered in the system. This includes only components written in
/// the C++ code. Other components can be registered during run-time.

#ifndef _COMPONENTENUMS_H_
#define _COMPONENTENUMS_H_

namespace EntityComponents {}
using namespace EntityComponents;

namespace EntitySystem
{
	/// A set of types of all components in the system. This includes only components written in
	/// the C++ code. Other components can be registered during run-time.
	enum eComponentType
	{
		/// Macro used for easier definition of components. The first parameter is the type ID while the second one is 
		/// the component class name.
		#define COMPONENT_TYPE(x, y) x,
		#include "../Components/_ComponentTypes.h"
		#undef COMPONENT_TYPE

		NUM_COMPONENT_TYPES,
		CT_INVALID
	};

	/// String names of all component types.
	const char* const ComponentTypeNames[] =
	{
		/// Macro used for easier definition of components. The first parameter is the type ID while the second one is 
		/// the component class name.
		#define COMPONENT_TYPE(x, y) #x,
		#include "../Components/_ComponentTypes.h"
		#undef COMPONENT_TYPE
	};

	/// Returns a component type based on a string.
	eComponentType DetectComponentType(const string& type);

}

#endif