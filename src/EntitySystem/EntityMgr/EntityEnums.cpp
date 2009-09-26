#include "Common.h"
#include "EntityEnums.h"

using namespace EntitySystem;


EntitySystem::eEntityType EntitySystem::DetectEntityType( const String& type )
{
	const char* typeStr = type.c_str();
	for (int32 i=0; i<NUM_ENTITY_TYPES; ++i)
		if (strcmp(typeStr, EntityTypeNames[i]) == 0)
			return static_cast<eEntityType>(i);
	gLogMgr.LogMessage("DetectEntityType:Unknown type '", type, "'", LOG_ERROR);
	return ET_UNKNOWN;
}