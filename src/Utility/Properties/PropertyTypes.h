//
// PropertyTypes.h
//

#ifndef _PROPERTY_TYPES_H
#define _PROPERTY_TYPES_H

#include "../Settings.h"

enum ePropertyAccess { PROPACC_EDIT=1<<1, PROPACC_SCRIPT=1<<2 };

enum ePropertyType
{
	PROPTYPE_BOOL,
	PROPTYPE_INT8,
	PROPTYPE_INT16,
	PROPTYPE_INT32,
	PROPTYPE_INT64,
	PROPTYPE_FLOAT32,
	PROPTYPE_STRING,
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

template<class T> ePropertyType CPropertyType<T>::ms_TypeID = PROPTYPE_PTR;
template<> ePropertyType CPropertyType<bool>::ms_TypeID		= PROPTYPE_BOOL;
template<> ePropertyType CPropertyType<int8>::ms_TypeID		= PROPTYPE_INT8;
template<> ePropertyType CPropertyType<int16>::ms_TypeID	= PROPTYPE_INT16;
template<> ePropertyType CPropertyType<int32>::ms_TypeID	= PROPTYPE_INT32;
template<> ePropertyType CPropertyType<int64>::ms_TypeID	= PROPTYPE_INT64;
template<> ePropertyType CPropertyType<float32>::ms_TypeID	= PROPTYPE_FLOAT32;
template<> ePropertyType CPropertyType<char*>::ms_TypeID	= PROPTYPE_STRING;

#endif	// _PROPERTY_TYPES_H