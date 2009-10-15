/// @file
/// A complete property knowing what type of values it represents and what class does it belong to.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _PROPERTY_H
#define _PROPERTY_H

#include "TypedProperty.h"
#include "PropertyEnums.h"

namespace Reflection
{
	/// @brief Fully defined property class.
	/// @remarks
	/// The class specifies the property's name and its type. The getters/setters are bound using function pointers.
	///	The template parameter OwnerType is a class that the getter and setter are members of. The template
	///	parameter T is the property value type.
	template <class OwnerType, class T> class Property : public TypedProperty<T>
	{

	public:

		/// Type for the getter function of the property value.
		typedef T (OwnerType::*GetterType)( void ) const;

		/// Type for the setter function of the property value.
		typedef void (OwnerType::*SetterType)( T Value );

		/// Constructor.
		inline Property( const char* name, GetterType getter, SetterType setter, const PropertyAccessFlags accessFlags ):
		TypedProperty<T> (name, accessFlags),
			mGetter (getter),
			mSetter (setter) {}

		/// Returns the value of this property. The owner of this property must be specified.
		virtual T GetValue(RTTIBaseClass* owner)
		{
			if (!mGetter)
			{
				this->ReportWriteonlyProblem();
				return PropertyType<T>::GetDefaultValue();
			}
			return (((OwnerType*)owner)->*mGetter)();
		}
		/// Sets the value of this property. The owner of this property must be specified.
		virtual void SetValue(RTTIBaseClass* owner, T Value)
		{
			if ( !mSetter)
			{
				this->ReportReadonlyProblem();		
				return;
			}
			(((OwnerType*)owner)->*mSetter)( Value );
		}

	private:

		GetterType mGetter;
		SetterType mSetter;

	};
}

#endif	// _PROPERTY_H
