/// @file
/// Definitions of individual value type of properties.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _PROPERTY_ENUMS_H
#define _PROPERTY_ENUMS_H

#include "Base.h"

// These includes are needed because of the usage of default values for the property types.
#include "StringKey.h"
#include "PropertyFunctionParameters.h"
#include "../../EntitySystem/EntityMgr/EntityHandle.h"
#include "../../GfxSystem/GfxStructures.h"

namespace Reflection
{
	/// @brief Value type of a property.
	/// @remarks It is used for runtime type checks.
	enum ePropertyType
	{
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue) typeID,
		#include "PropertyTypes.h"
		#undef PROPERTY_TYPE

		PT_UNKNOWN,
		NUM_PROPERTY_TYPES
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


#ifdef __WIN__
	template<class T> ePropertyType PropertyType<T>::mTypeID = PT_UNKNOWN;
	template<class T> T PropertyType<T>::mDefaultValue = 0;

	/// Template specialization.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue) \
		template<> ePropertyType PropertyType<typeClass>::mTypeID = typeID; \
		template<> typeClass PropertyType<typeClass>::mDefaultValue(defaultValue);
	#include "PropertyTypes.h"
	#undef PROPERTY_TYPE
#endif

}

#endif	// _PROPERTY_ENUMS_H
