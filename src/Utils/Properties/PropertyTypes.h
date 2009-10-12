/// @file
/// Definitions of individual value type of properties.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _PROPERTY_TYPES_H
#define _PROPERTY_TYPES_H

#include "Base.h"
#include "PropertyFunctionParameters.h"
#include "../../EntitySystem/EntityMgr/EntityHandle.h"
#include "../../GfxSystem/GfxStructures.h"

namespace Reflection
{
	/// @brief Value type of a property.
	/// @remarks It is used for runtime type checks.
	enum ePropertyType
	{
		PROPTYPE_BOOL,
		PROPTYPE_INT8,
		PROPTYPE_INT16,
		PROPTYPE_INT32,
		PROPTYPE_INT64,
		PROPTYPE_UINT8,
		PROPTYPE_UINT16,
		PROPTYPE_UINT32,
		PROPTYPE_UINT64,
		PROPTYPE_FLOAT32,
		PROPTYPE_VECTOR2,
		PROPTYPE_VECTOR2_REFERENCE,
		PROPTYPE_VECTOR2_ARRAY,
		PROPTYPE_STRING,
		PROPTYPE_STRING_KEY,
		PROPTYPE_ENTITYHANDLE,
		PROPTYPE_ENTITYHANDLE_ARRAY,
		PROPTYPE_COLOR,
		PROPTYPE_FUNCTION_PARAMETER,
		PROPTYPE_UNKNOWN,

		PROPTYPE_MAX
	};

	/// @brief This templatized class will associate compile-time types with unique enum members.
	template <class T> class PropertyType
	{
	public :

		/// Returns the type ID associated with the templatized type.
		static ePropertyType GetTypeID(void) { return mTypeID; };

		/// Returns the default value of this property type.
		static T GetDefaultValue(void) { return mDefaultValue; }

	private:

		static ePropertyType mTypeID;
		static T mDefaultValue;

	};
}

#endif	// _PROPERTY_TYPES_H
