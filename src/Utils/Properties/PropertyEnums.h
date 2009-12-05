/// @file
/// Definitions of individual value type of properties.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _PROPERTY_ENUMS_H
#define _PROPERTY_ENUMS_H

#include "Base.h"

namespace Reflection
{
	// Macros for automatically declaring array property types based on common types.
	#define ARRAY_PROPERTY_TYPE_ID(typeID) typeID##_ARRAY
	#define ARRAY_PROPERTY_TYPE_NAME(typeName) "Array<"typeName">*"
	#define ARRAY_PROPERTY_TYPE_CLASS(typeClass) Array<typeClass>*

	/// Value type of a property.
	/// It is used for runtime type checks.
	enum ePropertyType
	{
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) typeID,
		#include "PropertyTypes.h"
		#undef PROPERTY_TYPE

		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) ARRAY_PROPERTY_TYPE_ID(typeID),
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
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) \
			template<> ePropertyType GetTypeID<typeClass>(void); \
			template<> typeClass GetDefaultValue<typeClass>(void);
		#include "PropertyTypes.h"
		#undef PROPERTY_TYPE

		/// Template specialization.
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) \
			template<> ePropertyType GetTypeID<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(void); \
			template<> ARRAY_PROPERTY_TYPE_CLASS(typeClass) GetDefaultValue<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(void);
		#include "PropertyTypes.h"
		#undef PROPERTY_TYPE

		/// String names of property types.
		const char* const PropertyTypeNames[] =
		{
			#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) typeName,
			#include "PropertyTypes.h"
			#undef PROPERTY_TYPE

			#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) ARRAY_PROPERTY_TYPE_NAME(typeName),
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
