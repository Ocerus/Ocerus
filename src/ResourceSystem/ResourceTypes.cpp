/// @file
/// Definitions of resource types.

#include "Common.h"
#include "ResourceTypes.h"

using namespace ResourceSystem;

const char* const BasePathTypeName[] =
{
	"System", "Project", "Unknown", "Absolute"
};

const char* ResourceSystem::GetBasePathTypeName(const eBasePathType basePathType)
{
	return BasePathTypeName[basePathType];
}

string ResourceSystem::GetResourceTypeName( const eResourceType type )
{
	return ResourceTypeNames[type];
}

ResourceSystem::eResourceType ResourceSystem::GetResourceTypeFromString(const string& resourceTypeString)
{
	for (int i = 0; i < NUM_RESTYPES; ++i)
	{
		if (resourceTypeString == ResourceTypeNames[i])
			return (eResourceType)i;
	}
	return RESTYPE_UNKNOWN;
}
