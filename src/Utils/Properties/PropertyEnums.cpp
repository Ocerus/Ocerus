/// @file
/// Definitions of individual value type of properties.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#include "Common.h"
#include "PropertyEnums.h"

// These includes are needed because of the usage of default values for the property types.
#include "StringKey.h"
#include "PropertyFunctionParameters.h"
#include "../../EntitySystem/EntityMgr/EntityHandle.h"
#include "../../GfxSystem/GfxStructures.h"

namespace Reflection
{

	/// Template specialization.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName) \
		template<> ePropertyType PropertyTypes::GetTypeID<typeClass>(void) { return typeID; } \
		template<> typeClass PropertyTypes::GetDefaultValue<typeClass>(void) {return defaultValue; }
	#include "PropertyTypes.h"
	#undef PROPERTY_TYPE

	/// Template specialization.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName) \
		template<> ePropertyType PropertyTypes::GetTypeID<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(void) { return ARRAY_PROPERTY_TYPE_ID(typeID); } \
		template<> ARRAY_PROPERTY_TYPE_CLASS(typeClass) PropertyTypes::GetDefaultValue<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(void) {return 0; }
	#include "PropertyTypes.h"
	#undef PROPERTY_TYPE


	const char* PropertyTypes::GetStringName( const ePropertyType type )
	{
		return PropertyTypeNames[type];
	}
}

