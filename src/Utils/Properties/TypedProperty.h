/// @file
/// A property knowing what type of values it represents.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _TYPED_PROPERTY_H
#define _TYPED_PROPERTY_H

#include "AbstractProperty.h"

namespace Reflection
{
	/// This intermediate class defines a property that is typed, but not bound as a member of a particular	class.
	template <class T> class TypedProperty: public AbstractProperty
	{
	public:

		/// Constructor.
		/// @param accessFlags This parameter controls access to this property. It's similar to visibility in C++.
		inline TypedProperty(StringKey name, const PropertyAccessFlags accessFlags):
		AbstractProperty(name, accessFlags) {}

		/// Returns the type of this property.
		virtual ePropertyType GetType(void) const { return PropertyTypes::GetTypeID<T>(); }

		/// Returns true if the properties' values are equal. The properties must be of the same type.
		virtual bool IsEqual(RTTIBaseClass* owner, const RTTIBaseClass* otherOwner, const AbstractProperty* otherProperty)
		{
			if (!IsReadable() || !otherProperty->IsReadable())
				return false;

			return GetValue(owner) == otherProperty->GetValue<T>(otherOwner);
		}

		/// Copies data from the specified abstract property. The property must be of the same type as this property.
		/// Returns true if the copy was performed, false otherwise.
		virtual bool CopyFrom(RTTIBaseClass* owner, const RTTIBaseClass* otherOwner, const AbstractProperty* otherProperty)
		{
			if (!IsWriteable() || !otherProperty->IsReadable())
				return false;
			SetValue(owner, otherProperty->GetValue<T>(otherOwner));
			return true;
		}

		/// Returns the value of this property. An owner of the property must be specified.
		virtual T GetValue( const RTTIBaseClass* owner ) const = 0;

		/// Sets this property to a specified value. An owner of the property must be specified.
		virtual void SetValue( RTTIBaseClass* owner, T value ) = 0;

	};
}

#endif	// _TYPED_PROPERTY_H