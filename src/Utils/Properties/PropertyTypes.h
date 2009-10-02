#ifndef _PROPERTY_TYPES_H
#define _PROPERTY_TYPES_H

#include "Settings.h"
//TODO nejde to jen s forward deklaraci?
#include "../../EntitySystem/EntityMgr/EntityHandle.h"
#include "StringKey.h"
#include "../../GfxSystem/GfxStructures.h"
#include "PropertyFunctionParameters.h"

/** @name Type of access which will be granted to a property. It can be access from scripts, from editor, 
	during the init, ...
	Note that it is assumed that this type fits into PropertyAccessFlags!
*/
enum ePropertyAccess { PROPACC_EDIT_READ=1<<1, PROPACC_EDIT_WRITE=1<<2, PROPACC_SCRIPT_READ=1<<3, PROPACC_SCRIPT_WRITE=1<<4, PROPACC_INIT=1<<5 };

/// @name Type of a property. It is used for runtime type checks.
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

/// @name This templatized class will associate compile-time types with unique enum members.
template <class T> class PropertyType
{

public :

	/// @name Returns type ID associated with the templatized type.
	static ePropertyType GetTypeID(void) { return mTypeID; };

	/// @name Returns a default value of this property type.
	static T GetDefaultValue(void) { return mDefaultValue; }

private:

    static ePropertyType mTypeID;
	static T mDefaultValue;

};

/** @name Template specializations to set up types.
	Note: All changes here must be made in PropertyHolder as well!
*/
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

#endif	// _PROPERTY_TYPES_H