//
// AbstractProperty.h
//

#ifndef _ABSTRACT_PROPERTY_H
#define _ABSTRACT_PROPERTY_H

#include "PropertyTypes.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CAbstractProperty
//
// Base class for all properties.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


/// @name  Forward declarations.
//@{
class RTTIBaseClass;
//@}

class CAbstractProperty
{

public:

	//----------------------------------------------------------------------------------------------
	// Constructor. Takes in property name.
	inline CAbstractProperty( const char* szName, const uint8 accessFlags );

	//----------------------------------------------------------------------------------------------
	// Returns the name of this property.
	inline const char*              GetName() const;

	//----------------------------------------------------------------------------------------------
	// Returns the type of this property.
	virtual ePropertyType			GetType() const = 0;

	inline uint8	GetAccessFlags( void ) const { return m_accessFlags; }

	template<class T>
	T GetValue(const RTTIBaseClass* owner)
	{
		ePropertyType desiredType = CPropertyType<T>::GetTypeID();
		ePropertyType myType = GetType();
		if (myType != desiredType)
		{
			ReportConvertProblem(desiredType);
			return CPropertyType<T>::GetDefaultValue();
		}
		return ((CTypedProperty<T>*)this)->GetValue( owner );
	}

	template<class T>
	void SetValue(RTTIBaseClass* owner, const T value)
	{
		ePropertyType desiredType = CPropertyType<T>::GetTypeID();
		if (GetType() != desiredType)
		{
			ReportConvertProblem(desiredType);
			return;
		}
		return ((CTypedProperty<T>*)this)->SetValue( owner, value );
	}

    
protected :

	const char*				m_szName;			// Property name
	uint8			m_accessFlags;

	void ReportConvertProblem(ePropertyType wrongType);
	void ReportReadonlyProblem(void);
	void ReportWriteonlyProblem(void);

private:

};

typedef std::vector<CAbstractProperty*> AbstractPropertyList;


inline CAbstractProperty::CAbstractProperty( const char* szName, const uint8 accessFlags ) :
	m_szName		( szName			),
	m_accessFlags	( accessFlags )
{
}

inline const char* CAbstractProperty::GetName() const
{
	return m_szName;
}

#endif	// _ABSTRACT_PROPERTY_H