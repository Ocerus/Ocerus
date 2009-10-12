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

	template<> ePropertyType PropertyType<bool>::mTypeID;
	template<> ePropertyType PropertyType<int8>::mTypeID;
	template<> ePropertyType PropertyType<int16>::mTypeID;
	template<> ePropertyType PropertyType<int32>::mTypeID;
	template<> ePropertyType PropertyType<int64>::mTypeID;
	template<> ePropertyType PropertyType<uint8>::mTypeID;
	template<> ePropertyType PropertyType<uint16>::mTypeID;
	template<> ePropertyType PropertyType<uint32>::mTypeID;
	template<> ePropertyType PropertyType<uint64>::mTypeID;	
	template<> ePropertyType PropertyType<float32>::mTypeID;
	template<> ePropertyType PropertyType<Vector2>::mTypeID;
	template<> ePropertyType PropertyType<Vector2&>::mTypeID;
	template<> ePropertyType PropertyType<Vector2*>::mTypeID;
	template<> ePropertyType PropertyType<EntitySystem::EntityHandle>::mTypeID;
	template<> ePropertyType PropertyType<EntitySystem::EntityHandle*>::mTypeID;
	template<> ePropertyType PropertyType<char*>::mTypeID;
	template<> ePropertyType PropertyType<StringKey>::mTypeID;
	template<> ePropertyType PropertyType<GfxSystem::Color>::mTypeID;
	template<> ePropertyType PropertyType<PropertyFunctionParameters&>::mTypeID;

//	template<class T> T PropertyType<T>::mDefaultValue;
	template<> bool PropertyType<bool>::mDefaultValue;
	template<> int8 PropertyType<int8>::mDefaultValue;
	template<> int16 PropertyType<int16>::mDefaultValue;
	template<> int32 PropertyType<int32>::mDefaultValue;
	template<> int64 PropertyType<int64>::mDefaultValue;
	template<> uint8 PropertyType<uint8>::mDefaultValue;
	template<> uint16 PropertyType<uint16>::mDefaultValue;
	template<> uint32 PropertyType<uint32>::mDefaultValue;
	template<> uint64 PropertyType<uint64>::mDefaultValue;
	template<> float32 PropertyType<float32>::mDefaultValue;
	template<> Vector2 PropertyType<Vector2>::mDefaultValue;
	template<> Vector2& PropertyType<Vector2&>::mDefaultValue;
	template<> Vector2* PropertyType<Vector2*>::mDefaultValue;
	template<> EntitySystem::EntityHandle PropertyType<EntitySystem::EntityHandle>::mDefaultValue;
	template<> EntitySystem::EntityHandle* PropertyType<EntitySystem::EntityHandle*>::mDefaultValue; 
	template<> char* PropertyType<char*>::mDefaultValue;
	template<> StringKey PropertyType<StringKey>::mDefaultValue;
	template<> GfxSystem::Color PropertyType<GfxSystem::Color>::mDefaultValue;
	template<> PropertyFunctionParameters& PropertyType<PropertyFunctionParameters&>::mDefaultValue;

}

#endif	// _PROPERTY_TYPES_H
