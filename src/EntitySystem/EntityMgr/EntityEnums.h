/// @file
/// Definitions of all entity types to be registered in the system. This includes only entities defined in
/// the C++ code. Other entities can be registered during run-time.

#ifndef EntityEnums_h__
#define EntityEnums_h__

#include "Base.h"

namespace EntitySystem
{
	/// Set of all entity types in the system. This includes only entities defined in
	/// the C++ code. Other entities can be registered during run-time.
	enum eEntityType
	{
		#define ENTITY_TYPE(x) x,
		#include "EntityTypes.h"
		#undef ENTITY_TYPE

		NUM_ENTITY_TYPES
	};

	/// String names of all entity types.
	const char* const EntityTypeNames[] =
	{
		#define ENTITY_TYPE(x) #x,
		#include "EntityTypes.h"
		#undef ENTITY_TYPE
	};

	/// Returns an entity type based on a string.
	eEntityType DetectEntityType(const string& type);

}

#endif // EntityEnums_h__