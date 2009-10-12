/// @file
/// Definitions of individual value type of properties.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#include "PropertyTypes.h"
#include "StringKey.h"

namespace Reflection
{
	/// @name Type definitions.
	//@{
	template<class T> ePropertyType PropertyType<T>::mTypeID					= PROPTYPE_UNKNOWN;
	template<> ePropertyType PropertyType<bool>::mTypeID						= PROPTYPE_BOOL;
	template<> ePropertyType PropertyType<int8>::mTypeID						= PROPTYPE_INT8;
	template<> ePropertyType PropertyType<int16>::mTypeID						= PROPTYPE_INT16;
	template<> ePropertyType PropertyType<int32>::mTypeID						= PROPTYPE_INT32;
	template<> ePropertyType PropertyType<int64>::mTypeID						= PROPTYPE_INT64;
	template<> ePropertyType PropertyType<uint8>::mTypeID						= PROPTYPE_UINT8;
	template<> ePropertyType PropertyType<uint16>::mTypeID						= PROPTYPE_UINT16;
	template<> ePropertyType PropertyType<uint32>::mTypeID						= PROPTYPE_UINT32;
	template<> ePropertyType PropertyType<uint64>::mTypeID						= PROPTYPE_UINT64;
	template<> ePropertyType PropertyType<float32>::mTypeID						= PROPTYPE_FLOAT32;
	template<> ePropertyType PropertyType<Vector2>::mTypeID						= PROPTYPE_VECTOR2;
	template<> ePropertyType PropertyType<Vector2&>::mTypeID					= PROPTYPE_VECTOR2_REFERENCE;
	template<> ePropertyType PropertyType<Vector2*>::mTypeID					= PROPTYPE_VECTOR2_ARRAY;
	template<> ePropertyType PropertyType<EntitySystem::EntityHandle>::mTypeID	= PROPTYPE_ENTITYHANDLE;
	template<> ePropertyType PropertyType<EntitySystem::EntityHandle*>::mTypeID	= PROPTYPE_ENTITYHANDLE_ARRAY;
	template<> ePropertyType PropertyType<char*>::mTypeID						= PROPTYPE_STRING;
	template<> ePropertyType PropertyType<StringKey>::mTypeID					= PROPTYPE_STRING_KEY;
	template<> ePropertyType PropertyType<GfxSystem::Color>::mTypeID			= PROPTYPE_COLOR;
	template<> ePropertyType PropertyType<PropertyFunctionParameters&>::mTypeID	= PROPTYPE_FUNCTION_PARAMETER;

	template<class T> T PropertyType<T>::mDefaultValue							= 0;
	template<> bool PropertyType<bool>::mDefaultValue							= false;
	template<> int8 PropertyType<int8>::mDefaultValue							= 0;
	template<> int16 PropertyType<int16>::mDefaultValue							= 0;
	template<> int32 PropertyType<int32>::mDefaultValue							= 0;
	template<> int64 PropertyType<int64>::mDefaultValue							= 0;
	template<> uint8 PropertyType<uint8>::mDefaultValue							= 0;
	template<> uint16 PropertyType<uint16>::mDefaultValue						= 0;
	template<> uint32 PropertyType<uint32>::mDefaultValue						= 0;
	template<> uint64 PropertyType<uint64>::mDefaultValue						= 0;
	template<> float32 PropertyType<float32>::mDefaultValue						= 0.f;
	template<> Vector2 PropertyType<Vector2>::mDefaultValue						= Vector2_Zero;
	template<> Vector2& PropertyType<Vector2&>::mDefaultValue					= Vector2_Dummy;
	template<> Vector2* PropertyType<Vector2*>::mDefaultValue					= 0;
	template<> EntitySystem::EntityHandle PropertyType<EntitySystem::EntityHandle>::mDefaultValue = EntitySystem::EntityHandle::Null;
	template<> EntitySystem::EntityHandle* PropertyType<EntitySystem::EntityHandle*>::mDefaultValue = 0;
	template<> char* PropertyType<char*>::mDefaultValue							= 0;
	template<> StringKey PropertyType<StringKey>::mDefaultValue					= StringKey();
	template<> GfxSystem::Color PropertyType<GfxSystem::Color>::mDefaultValue	= GfxSystem::Color::NullColor;
	template<> PropertyFunctionParameters& PropertyType<PropertyFunctionParameters&>::mDefaultValue = PropertyFunctionParameters::None;
	//@}
}

