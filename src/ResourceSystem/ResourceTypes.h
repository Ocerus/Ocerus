/// @file
/// Custom types of resources.

#ifndef ResourceTypes_h__
#define ResourceTypes_h__

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
		RESTYPE_PARTICLERESOURCE,
		RESTYPE_SCRIPTRESOURCE,
		RESTYPE_UNKNOWN,
		
		NUM_RESTYPES, 
		RESTYPE_AUTODETECT
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
}


#endif // ResourceTypes_h__