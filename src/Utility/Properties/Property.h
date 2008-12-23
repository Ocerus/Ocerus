#ifndef _PROPERTY_H
#define _PROPERTY_H

#include "TypedProperty.h"
#include "PropertyTypes.h"

/** Fully defined property class. Specifies property's name, type and the getter/setters are bound as
	members of a specific class type.
	Template parameter OwnerType is the class that the getter and setter are a member of and template
	parameter T is the property type.
*/
template <class OwnerType, class T> class Property : public TypedProperty<T>
{

public:

	/// @name getter type.
	typedef T (OwnerType::*GetterType)( void ) const;
	/// @name setter type.
	typedef void (OwnerType::*SetterType)( T Value );

	/// @name Constructor.
	inline Property( const char* name, GetterType getter, SetterType setter, const uint8 accessFlags ):
		TypedProperty<T> (name, accessFlags),
		mGetter (getter),
		mSetter (setter) {}

	/// @name Returns the value of this property. Owner of this property must be specified.
	virtual T GetValue(RTTIBaseClass* owner)
	{
		if (!mGetter)
		{
			ReportWriteonlyProblem();
			return PropertyType<T>::GetDefaultValue();
		}
		return (((OwnerType*)owner)->*mGetter)();
	}
	/// @name Sets the value of this property. Owner must be specified.
	virtual void SetValue(RTTIBaseClass* owner, T Value)
	{
		if ( !mSetter)
		{
			ReportReadonlyProblem();		
			return;
		}
		(((OwnerType*)owner)->*mSetter)( Value );
	}

protected:

    GetterType mGetter;
    SetterType mSetter;

};

#endif	// _PROPERTY_H