#include "Common.h"
#include "ComponentEnums.h"


EntitySystem::eComponentType EntitySystem::DetectComponentType( const string& type )
{
	const char* typeStr = type.c_str();
	for (int32 i=0; i<NUM_COMPONENT_TYPES; ++i)
		if (strcmp(typeStr, ComponentTypeNames[i]) == 0)
			return static_cast<eComponentType>(i);
	ocError << "DetectComponentType: Unknown type '" << type << "'";
	return CT_INVALID;
}

string EntitySystem::GetComponentTypeName(const eComponentType type)
{
	return ComponentTypeNames[type];
}
