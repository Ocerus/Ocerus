/// @file
/// Abstract representation of a property.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#ifndef _ABSTRACT_PROPERTY_H
#define _ABSTRACT_PROPERTY_H

#include "Base.h"
#include "StringKey.h"
#include "PropertyAccess.h"
#include "PropertyEnums.h"

namespace Reflection
{
    /// Base class for all properties.
	/// A property represents either a field or a function of a class instance. The key advantage of properties
	/// is that you access all of them in the same transparent way - using a string identifier. For example, this can be used to access
	/// the properties from a script or a GUI editor in a very easy way.
	class AbstractProperty
	{
	public:

		/// Constructor.
		/// @param accessFlags This parameter controls access to this property. It's similar to visibility in C++.
		inline AbstractProperty( const char* szName, const PropertyAccessFlags accessFlags ):
			mKey(szName),
			mName(szName),
			mAccessFlags(accessFlags) {}

		/// Returns the name of this property.
		inline const char* GetName(void) const { return mName; }

		/// Returns the comment of this property or empty string if no comment was set.
		inline string GetComment(void) const { return mComment; }

		/// Sets the comment for this property.
		inline void SetComment(const string& comment) { mComment = comment; }

		/// Returns string key created from the name of this property.
		inline StringKey GetKey(void) const { return mKey; }

		/// Returns the type of this property.
		virtual ePropertyType GetType(void) const = 0;

		/// Returns flags indicating what is accessible in this property.
		inline PropertyAccessFlags GetAccessFlags(void) const { return mAccessFlags; }

		/// Returns true if the property can be read.
		virtual bool IsReadable(void) const = 0;

		/// Returns true if the property can be written.
		virtual bool IsWriteable(void) const = 0;

		/// Copies data from the specified abstract property. The property must be of the same type as this property.
		/// Returns true if the copy was performed, false otherwise.
		virtual bool CopyFrom(RTTIBaseClass* owner, const RTTIBaseClass* otherOwner, const AbstractProperty* otherProperty) = 0;

		/// Returns the value of this property. An owner of the property must be specified.
		template<class T>
		T GetValue(const RTTIBaseClass* owner) const
		{
			ePropertyType desiredType = PropertyTypes::GetTypeID<T>();
			ePropertyType myType = GetType();
			if (myType != desiredType)
			{
				ReportConvertProblem(desiredType);
				return PropertyTypes::GetDefaultValue<T>();
			}
			return ((TypedProperty<T>*)this)->GetValue( owner );
		}

		/// Sets this property to a specified value. An owner of the property must be specified.
		template<class T>
		void SetValue(RTTIBaseClass* owner, const T value)
		{
			ePropertyType desiredType = PropertyTypes::GetTypeID<T>();
			if (GetType() != desiredType)
			{
				ReportConvertProblem(desiredType);
				return;
			}
			return ((TypedProperty<T>*)this)->SetValue( owner, value );
		}

		/// Parses the typed value of this property from the input string.
		void SetValueFromString(RTTIBaseClass* owner, const string& str);

	protected:

		/// Reports an error when the property is accessed with a wrong type and cannot be converted between these types.
		void ReportConvertProblem(ePropertyType wrongType) const;

		/// Reports an error when the property is being read, but doesn't support reading data.
		void ReportReadonlyProblem(void) const;

		/// Reports an error when the property is being written, but doesn't support writing data.
		void ReportWriteonlyProblem(void) const;

	private :

		StringKey mKey;
		const char* mName;
		string mComment;
		PropertyAccessFlags mAccessFlags;

	};
}

#endif	// _ABSTRACT_PROPERTY_H
