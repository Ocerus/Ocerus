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
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
		template<> ePropertyType PropertyTypes::GetTypeID<typeClass>(void) { return typeID; } \
		template<> typeClass PropertyTypes::GetDefaultValue<typeClass>(void) {return defaultValue; }
	#include "PropertyTypes.h"
	#undef PROPERTY_TYPE

	/// Template specialization.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
		template<> ePropertyType PropertyTypes::GetTypeID<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(void) { return ARRAY_PROPERTY_TYPE_ID(typeID); } \
		template<> ARRAY_PROPERTY_TYPE_CLASS(typeClass) PropertyTypes::GetDefaultValue<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(void) {return 0; }
	#include "PropertyTypes.h"
	#undef PROPERTY_TYPE


	const char* PropertyTypes::GetStringName( const ePropertyType type )
	{
		return PropertyTypeNames[type];
	}

	ePropertyType PropertyTypes::GetTypeFromName(const string& name)
	{
		for (int32 i = 0; i < NUM_PROPERTY_TYPES; ++i)
		{
			if (name.compare(PropertyTypeNames[i]) == 0) { return (ePropertyType)i; }
		}
		return PT_UNKNOWN;
	}

	ePropertyCloning PropertyTypes::GetCloning( const ePropertyType type )
	{
		switch (type)
		{
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
			case typeID: return PC_##cloning; \
			case ARRAY_PROPERTY_TYPE_ID(typeID): return PC_##cloning;
		#include "PropertyTypes.h"
		#undef PROPERTY_TYPE
		default: OC_NOT_REACHED();
		}

		OC_FAIL("Uknown property cloning");

		return PC_SHALLOW;
	}

}

