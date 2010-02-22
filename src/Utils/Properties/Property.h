/// @file
/// A complete property knowing what type of values it represents and what class does it belong to.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _PROPERTY_H
#define _PROPERTY_H

#include "TypedProperty.h"
#include "PropertyEnums.h"

#ifdef __WIN__
#pragma warning(push)
#pragma warning (disable: 4121)
#endif

namespace Reflection
{
	/// Fully defined property class.
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
		inline Property(StringKey name, GetterType getter, SetterType setter, const PropertyAccessFlags accessFlags, const string& comment):
		TypedProperty<T> (name, accessFlags),
			mGetter (getter),
			mSetter (setter)
		{
			AbstractProperty::SetComment(comment);
		}

		/// Returns true if the property can be read.
		virtual bool IsReadable(void) const
		{
			return mGetter != 0;
		}

		/// Returns true if the property can be written.
		virtual bool IsWriteable(void) const
		{
			return mSetter != 0;
		}

		/// Returns the value of this property. The owner of this property must be specified.
		virtual T GetValue(const RTTIBaseClass* owner) const
		{
			if (!mGetter)
			{
				this->ReportWriteonlyProblem();
				return PropertyTypes::GetDefaultValue<T>();
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

#ifdef __WIN__
#pragma warning(pop)
#endif


#endif	// _PROPERTY_H
