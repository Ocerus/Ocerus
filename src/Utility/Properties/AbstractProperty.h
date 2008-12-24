#ifndef _ABSTRACT_PROPERTY_H
#define _ABSTRACT_PROPERTY_H

#include "PropertyTypes.h"
#include "../StringKey.h"
#include <vector>

/// @name  Forward declarations.
//@{
class RTTIBaseClass;
//@}

/// @name Base class for all properties.
class AbstractProperty
{

public:

	/// @name Constructor. Takes in property name.
	inline AbstractProperty( const char* szName, const PropertyAccessFlags accessFlags ):
		mKey(szName),
		mName(szName),
		mAccessFlags(accessFlags) {}

	/// @name Returns the name of this property.
	inline const char* GetName(void) const { return mName; }

	/// @name Returns string key created from name.
	inline StringKey GetKey(void) const { return mKey; }

	/// @name Returns the type of this property.
	virtual ePropertyType GetType(void) const = 0;

	/// @name Returns flags indicating what is accessible in this property.
	inline PropertyAccessFlags GetAccessFlags(void) const { return mAccessFlags; }

	/// @name Returns the value of this property. An owner of the property must be specified.
	template<class T>
	T GetValue(const RTTIBaseClass* owner)
	{
		ePropertyType desiredType = PropertyType<T>::GetTypeID();
		ePropertyType myType = GetType();
		if (myType != desiredType)
		{
			ReportConvertProblem(desiredType);
			return PropertyType<T>::GetDefaultValue();
		}
		return ((TypedProperty<T>*)this)->GetValue( owner );
	}

	/// @name Sets this property to a specified value. An owner of the property must be specified.
	template<class T>
	void SetValue(RTTIBaseClass* owner, const T value)
	{
		ePropertyType desiredType = PropertyType<T>::GetTypeID();
		if (GetType() != desiredType)
		{
			ReportConvertProblem(desiredType);
			return;
		}
		return ((TypedProperty<T>*)this)->SetValue( owner, value );
	}

    
protected :

	StringKey mKey;
	const char* mName;
	PropertyAccessFlags mAccessFlags;

	/// @name Error reporting internals.
	//@{
	void ReportConvertProblem(ePropertyType wrongType);
	void ReportReadonlyProblem(void);
	void ReportWriteonlyProblem(void);
	//@}

};

/// @name A list of generic properties.
typedef std::vector<AbstractProperty*> AbstractPropertyList;

#endif	// _ABSTRACT_PROPERTY_H