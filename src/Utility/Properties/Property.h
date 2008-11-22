//
// Property.h
//

#ifndef _PROPERTY_H
#define _PROPERTY_H

#include "TypedProperty.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CProperty
//
// Fully defined property class. Specifies property's name, type and the getter/setters are bound as
// members of a specific class type.
//
// Template parameter OwnerType is the class that the getter and setter are a member of and template
// parameter T is the property type.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class OwnerType, class T> class CProperty : public CTypedProperty<T>
{

public:

	//----------------------------------------------------------------------------------------------
	typedef T		(OwnerType::*GetterType)( void ) const;		// Getter function
	typedef void	(OwnerType::*SetterType)( T Value );		// Setter function

	//----------------------------------------------------------------------------------------------
	// Constructor. Takes in property's name, getter and setter functions.
	inline CProperty( const char* szName, GetterType Getter, SetterType Setter, const uint8 accessFlags );

	//----------------------------------------------------------------------------------------------
	// Determines the value of this property.
	virtual T		GetValue( RTTIBaseClass* pObject );
	virtual void	SetValue( RTTIBaseClass* pObject, T Value );
	inline uint8	GetAccessFlags( void ) const { return m_accessFlags; }

protected:

    GetterType		m_Getter;
    SetterType		m_Setter;
	uint8			m_accessFlags;
 
};

template <class OwnerType, class T>
inline CProperty<OwnerType, T>::CProperty( const char* szName, GetterType Getter, SetterType Setter, const uint8 accessFlags ) :
	CTypedProperty<T>	( szName		),
	m_Getter			( Getter		),
	m_Setter			( Setter		),
	m_accessFlags		( accessFlags	)
{
}

template <class OwnerType, class T> 
T CProperty<OwnerType, T>::GetValue( RTTIBaseClass* pOwner )
{
	return (((OwnerType*)pOwner)->*m_Getter)();
}

template <class OwnerType, class T> 
void CProperty<OwnerType, T>::SetValue(RTTIBaseClass* pOwner, T Value )
{
	if ( !m_Setter)
	{
		assert( false ); // Cannot write to a read-only property
		return;
	}
	(((OwnerType*)pOwner)->*m_Setter)( Value );
}

#endif	// _PROPERTY_H