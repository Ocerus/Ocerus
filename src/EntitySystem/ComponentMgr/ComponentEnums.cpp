#include "Common.h"
#include "ComponentEnums.h"


EntitySystem::eComponentType EntitySystem::DetectComponentType( const string& type )
{
	const char* typeStr = type.c_str();
	for (int32 i=0; i<NUM_COMPONENT_TYPES; ++i)
		if (strcmp(typeStr, ComponentTypeNames[i]) == 0)
			return static_cast<eComponentType>(i);
	gLogMgr.LogMessage("DetectComponentType:Unknown type '", type, "'", LOG_ERROR);
	return CT_INVALID;
}

string EntitySystem::GetComponentTypeName(const eComponentType type)
{
	return ComponentTypeNames[type];
}
