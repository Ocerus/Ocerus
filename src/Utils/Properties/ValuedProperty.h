/// @file
/// A complete property holding its data knowing what type of values it represents.

#ifndef _VALUED_PROPERTY_H
#define _VALUED_PROPERTY_H

#include "TypedProperty.h"
#include "PropertyEnums.h"

#ifdef __WIN__
#pragma warning(push)
#pragma warning (disable: 4121)
#endif

namespace Reflection
{
	/// Fully defined property class which holds its data.
	/// @remarks
	/// The class specifies the property's name and its type. The template
	///	parameter T is the property value type. It holds its data in private attribute.
	template <class T> class ValuedProperty : public TypedProperty<T>
	{
	public:

		/// Constructor.
		inline ValuedProperty(StringKey name, const PropertyAccessFlags accessFlags, const string& comment ):
		TypedProperty<T> (name, accessFlags)
		{
			AbstractProperty::SetComment(comment);
			mData = PropertyTypes::GetDefaultValue<T>();
		}

		/// Returns true if the property can be read.
		virtual bool IsReadable(void) const
		{
			return true;
		}

		/// Returns true if the property can be written.
		virtual bool IsWriteable(void) const
		{
			return true;
		}

		/// Returns true if the property holds its value.
		virtual bool IsValued(void) const
		{
			return true;
		}

		/// Returns the value of this property. The owner of this property must be specified.
		virtual T GetValue(const RTTIBaseClass* owner) const
		{
			OC_UNUSED(owner);
			return mData;
		}
		/// Sets the value of this property. The owner of this property must be specified.
		virtual void SetValue(RTTIBaseClass* owner, T Value)
		{
			OC_UNUSED(owner);
			mData = Value;
		}

	private:
		T mData;
	};
}

#ifdef __WIN__
#pragma warning(pop)
#endif


#endif	// _PROPERTY_H
