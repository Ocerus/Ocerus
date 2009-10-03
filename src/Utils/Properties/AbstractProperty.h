/// @file
/// Abstract representation of a property.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _ABSTRACT_PROPERTY_H
#define _ABSTRACT_PROPERTY_H

#include "PropertyTypes.h"
#include "TypedProperty.h"
#include "../StringKey.h"

namespace Reflection
{
	class RTTIBaseClass;

	/// @brief Base class for all properties.
	/// @remarks A property represents either a field or a function of a class instance. The key advantage of properties
	/// is that you access all of them in the same transparent way - using a string identifier. For example, this can be used to access
	/// the properties from a script or a GUI editor in a very easy way.
	class AbstractProperty
	{
	public:

		/// @brief Constructor.
		/// @param accessFlags This parameter controls access to this property. It's similar to visibility in C++.
		inline AbstractProperty( const char* szName, const PropertyAccessFlags accessFlags ):
		mKey(szName),
			mName(szName),
			mAccessFlags(accessFlags) {}

		/// Returns the name of this property.
		inline const char* GetName(void) const { return mName; }

		/// Returns string key created from the name of this property.
		inline StringKey GetKey(void) const { return mKey; }

		/// Returns the type of this property.
		virtual ePropertyType GetType(void) const = 0;

		/// Returns flags indicating what is accessible in this property.
		inline PropertyAccessFlags GetAccessFlags(void) const { return mAccessFlags; }

		/// Returns the value of this property. An owner of the property must be specified.
		template<class T>
		T GetValue(RTTIBaseClass* owner)
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

		/// Sets this property to a specified value. An owner of the property must be specified.
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

	protected:

		/// Reports an error when the property is accessed with a wrong type and cannot be converted between these types.
		void ReportConvertProblem(ePropertyType wrongType);

		/// Reports an error when the property is being read, but doesn't support reading data.
		void ReportReadonlyProblem(void);

		/// Reports an error when the property is being written, but doesn't support writing data.
		void ReportWriteonlyProblem(void);

	private :

		StringKey mKey;
		const char* mName;
		PropertyAccessFlags mAccessFlags;

	};

	/// A list of generic properties.
	typedef vector<AbstractProperty*> AbstractPropertyList;
}

#endif	// _ABSTRACT_PROPERTY_H