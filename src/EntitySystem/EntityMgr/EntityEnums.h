#ifndef EntityEnums_h__
#define EntityEnums_h__

#include "../../Utility/Settings.h"

namespace EntitySystem
{
	/// @name Set of all entities in the system
	enum eEntityType
	{
		#define ENTITY_TYPE(x) x,
		#include "EntityTypes.h"
		#undef ENTITY_TYPE

		NUM_ENTITY_TYPES
	};

	/// @name String names of all entity types.
	const char* const EntityTypeNames[] =
	{
		#define ENTITY_TYPE(x) #x,
		#include "EntityTypes.h"
		#undef ENTITY_TYPE
	};

	/// @name Returns an entity type based on a string.
	eEntityType DetectEntityType(const string& type);

}

#endif // EntityEnums_h__