#ifndef EntityEnums_h__
#define EntityEnums_h__

namespace EntitySystem
{
	/// Set of all entyties in the system
	enum eEntityType
	{
		ET_SHIP=0,
		ET_PLATFORM,
		ET_ENGINE,

		ET_UNKNOWN, // unknown is a valid entity type as well
		NUM_ENTITY_TYPES,
	};
}

#endif // EntityEnums_h__