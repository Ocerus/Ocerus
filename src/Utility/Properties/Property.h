//
// Property.h
//

#ifndef _PROPERTY_H
#define _PROPERTY_H

#include "TypedProperty.h"
#include "PropertyTypes.h"

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

protected:

    GetterType		m_Getter;
    SetterType		m_Setter;

};


template <class OwnerType, class T>
inline CProperty<OwnerType, T>::CProperty( const char* szName, GetterType Getter, SetterType Setter, const uint8 accessFlags ) :
	CTypedProperty<T>	( szName, accessFlags	),
	m_Getter			( Getter		),
	m_Setter			( Setter		)
{
}

template <class OwnerType, class T> 
T CProperty<OwnerType, T>::GetValue( RTTIBaseClass* pOwner )
{
	if (!m_Getter)
	{
		ReportWriteonlyProblem();
		return CPropertyType<T>::GetDefaultValue();
	}
	return (((OwnerType*)pOwner)->*m_Getter)();
}

template <class OwnerType, class T> 
void CProperty<OwnerType, T>::SetValue(RTTIBaseClass* pOwner, T Value )
{
	if ( !m_Setter)
	{
		ReportReadonlyProblem();		
		return;
	}
	(((OwnerType*)pOwner)->*m_Setter)( Value );
}

#endif	// _PROPERTY_H