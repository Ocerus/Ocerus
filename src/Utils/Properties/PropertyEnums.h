/// @file
/// Definitions of individual value type of properties.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _PROPERTY_ENUMS_H
#define _PROPERTY_ENUMS_H

#include "Base.h"

namespace Reflection
{
	/// Value type of a property.
	/// It is used for runtime type checks.
	enum ePropertyType
	{
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName) typeID,
		#include "PropertyTypes.h"
		#undef PROPERTY_TYPE

		PT_UNKNOWN,
		NUM_PROPERTY_TYPES
	};

	/// This namespace contains templatized functions associated in compile-time with specific property types and values.
	namespace PropertyTypes
	{
		/// Returns the type ID associated with the templatized type.
		template<typename T> ePropertyType GetTypeID(void) { return PT_UNKNOWN; };

		/// Returns the default value of this property type.
		template<typename T> T GetDefaultValue(void) { return 0; }

		/// Template specialization.
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName) \
			template<> ePropertyType GetTypeID<typeClass>(void); \
			template<> typeClass GetDefaultValue<typeClass>(void);
		#include "PropertyTypes.h"
		#undef PROPERTY_TYPE

		/// String names of property types.
		const char* const PropertyTypeNames[] =
		{
			#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName) typeName,
			#include "PropertyTypes.h"
			#undef PROPERTY_TYPE

			"Unknown",
			"NUM_PROPERTY_TYPES"
		};

		/// Returns the string name of this property type.
		const char* GetStringName(const ePropertyType type);

	}
}

#endif	// _PROPERTY_ENUMS_H
