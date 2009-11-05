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
		RESTYPE_CEGUIRESOURCE, 
		RESTYPE_TEXTRESOURCE, 
		RESTYPE_XMLRESOURCE, 
		RESTYPE_PARTICLERESOURCE,
		RESTYPE_SCRIPTRESOURCE,
		
		NUM_RESTYPES, 
		RESTYPE_AUTODETECT 
	};
}


#endif // ResourceTypes_h__