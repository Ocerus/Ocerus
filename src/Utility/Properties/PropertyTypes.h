//
// PropertyTypes.h
//

#ifndef _PROPERTY_TYPES_H
#define _PROPERTY_TYPES_H

#include "../Settings.h"
#include "../EntitySystem/EntityMgr/EntityHandle.h"

enum ePropertyAccess { PROPACC_EDIT_READ=1<<1, PROPACC_EDIT_WRITE=1<<2, PROPACC_SCRIPT_READ=1<<3, PROPACC_SCRIPT_WRITE=1<<4 };

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
	PROPTYPE_ENTITYHANDLE,
	PROPTYPE_ENTITYHANDLE_ARRAY,
	PROPTYPE_PTR,

	PROPTYPE_MAX
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CPropertyType
//
// This templatized class will associate compile-time types with unique enum members.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class CPropertyType
{

public :

	// Returns type ID associated with the templatized type.
	static ePropertyType GetTypeID();

private:

    static ePropertyType ms_TypeID;

};

template<class T>
ePropertyType CPropertyType<T>::GetTypeID()
{
	return ms_TypeID;
}

template<class T> ePropertyType CPropertyType<T>::ms_TypeID						= PROPTYPE_PTR;
template<> ePropertyType CPropertyType<bool>::ms_TypeID							= PROPTYPE_BOOL;
template<> ePropertyType CPropertyType<int8>::ms_TypeID							= PROPTYPE_INT8;
template<> ePropertyType CPropertyType<int16>::ms_TypeID						= PROPTYPE_INT16;
template<> ePropertyType CPropertyType<int32>::ms_TypeID						= PROPTYPE_INT32;
template<> ePropertyType CPropertyType<int64>::ms_TypeID						= PROPTYPE_INT64;
template<> ePropertyType CPropertyType<uint8>::ms_TypeID						= PROPTYPE_UINT8;
template<> ePropertyType CPropertyType<uint16>::ms_TypeID						= PROPTYPE_UINT16;
template<> ePropertyType CPropertyType<uint32>::ms_TypeID						= PROPTYPE_UINT32;
template<> ePropertyType CPropertyType<uint64>::ms_TypeID						= PROPTYPE_UINT64;
template<> ePropertyType CPropertyType<float32>::ms_TypeID						= PROPTYPE_FLOAT32;
template<> ePropertyType CPropertyType<Vector2>::ms_TypeID						= PROPTYPE_VECTOR2;
template<> ePropertyType CPropertyType<Vector2&>::ms_TypeID						= PROPTYPE_VECTOR2_REFERENCE;
template<> ePropertyType CPropertyType<Vector2*>::ms_TypeID						= PROPTYPE_VECTOR2_ARRAY;
template<> ePropertyType CPropertyType<EntitySystem::EntityHandle>::ms_TypeID	= PROPTYPE_ENTITYHANDLE;
template<> ePropertyType CPropertyType<EntitySystem::EntityHandle*>::ms_TypeID	= PROPTYPE_ENTITYHANDLE_ARRAY;
template<> ePropertyType CPropertyType<char*>::ms_TypeID						= PROPTYPE_STRING;

#endif	// _PROPERTY_TYPES_H