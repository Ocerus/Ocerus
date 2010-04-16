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