//
// TypedProperty.h
//

#ifndef _TYPED_PROPERTY_H
#define _TYPED_PROPERTY_H

#include "../Properties/AbstractProperty.h"

class RTTIBaseClass;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CTypedProperty
//
// This intermediate class defines a property that is typed, but not bound as a member of a particular
// class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class CTypedProperty: public CAbstractProperty
{

public:

	//----------------------------------------------------------------------------------------------
	// Constructor. Takes in property name.
	inline CTypedProperty( const char* szName, const uint8 accessFlags );

	//----------------------------------------------------------------------------------------------
	// Returns the type of this property.
	virtual ePropertyType	GetType() const;

	//----------------------------------------------------------------------------------------------
	// Determines the value of this property.
	virtual T		GetValue( RTTIBaseClass* pObject ) = 0;
	virtual void	SetValue( RTTIBaseClass* pObject, T Value ) = 0;

};

template <class T>
inline CTypedProperty<T>::CTypedProperty( const char* szName, const uint8 accessFlags ) :
	CAbstractProperty( szName, accessFlags			)
{
}

template <class T>
ePropertyType CTypedProperty<T>::GetType() const
{
	return CPropertyType<T>::GetTypeID();
}

#endif	// _TYPED_PROPERTY_H