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

		/// @brief Constructor.
		/// @param accessFlags This parameter controls access to this property. It's similar to visibility in C++.
		inline TypedProperty(const char* name, const PropertyAccessFlags accessFlags):
		AbstractProperty(name, accessFlags) {}

		/// Returns the type of this property.
		virtual ePropertyType GetType(void) const { return PropertyType<T>::GetTypeID(); }

		/// Returns the value of this property. An owner of the property must be specified.
		virtual T GetValue( RTTIBaseClass* owner ) = 0;

		/// Sets this property to a specified value. An owner of the property must be specified.
		virtual void SetValue( RTTIBaseClass* owner, T value ) = 0;

	};
}

#endif	// _TYPED_PROPERTY_H