/// @file
/// Custom types of resources.

#ifndef ResourceTypes_h__
#define ResourceTypes_h__

#include "Common.h"

namespace ResourceSystem
{
	/// Custom type of a resource. Add your types here!
	enum eResourceType 
	{ 
		RESTYPE_TEXTURE=0, 
		RESTYPE_MESH,
		RESTYPE_CEGUIRESOURCE, 
		RESTYPE_TEXTRESOURCE, 
		RESTYPE_XMLRESOURCE, 
		RESTYPE_SCRIPTRESOURCE,
		RESTYPE_UNKNOWN,
		
		NUM_RESTYPES, 
		RESTYPE_AUTODETECT
	};

	/// Names of a resource types.
	const char* const ResourceTypeNames[] = 
	{
		"Texture",
		"Mesh",
		"CEGUI",
		"Text",
		"XML",
		"Script",
		"Unknown",

		"NumRestypes",
		"Unknown"
	};
	
	/// Types of path. Relative to some base path or the last one is the absolute path.
	enum eBasePathType
	{
		BPT_SYSTEM = 0,
		BPT_PROJECT,
		
		NUM_BASEPATHTYPES,
		BPT_ABSOLUTE
	};
	
	/// String name of types of a base path.
	const char* GetBasePathTypeName(const eBasePathType basePathType);

	/// Returns a string representation of the given resource type.
	string GetResourceTypeName(const eResourceType type);

	/// Returns the resource type that corresponds with specified string, or RESTYPE_UNKNOWN.
	eResourceType GetResourceTypeFromString(const string&);

	/// Specifies resource types for given resources in project. The string used as the key is resource
	/// file path relative to the project root.
	/// @todo We may want to rename this type (I'm out of inspiration now).
	typedef map<string, eResourceType> ResourceTypeMap;

}


#endif // ResourceTypes_h__