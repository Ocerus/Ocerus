/// @file
/// This is a way how to interact with properties from the user code.

#ifndef PropertyHolder_h__
#define PropertyHolder_h__

#include "Base.h"
#include "AbstractProperty.h"
#include "../../Utils/Properties/PropertyFunctionParameters.h"

namespace Reflection
{
	/// This class encapsulates abstract properties to allow easier access to their values and/or calling functions.
	class PropertyHolder
	{
	public:

		/// Default constructor.
		PropertyHolder(void): mOwner(0), mProperty(0) {}

		/// Copy constructor.
		PropertyHolder(const PropertyHolder& rhs): mOwner(rhs.mOwner), mProperty(rhs.mProperty) {}

		/// Constructs the holder from an instance of a class the property is bound to and the actual property.
		PropertyHolder(RTTIBaseClass* owner, AbstractProperty* prop): mOwner(owner), mProperty(prop) {}

		/// Assignment operator.
		PropertyHolder& operator=(const PropertyHolder& rhs)
		{
			mOwner = rhs.mOwner;
			mProperty = rhs.mProperty;
			return *this;
		}

		/// Returns the access flags to this property.
		inline PropertyAccessFlags GetAccessFlags(void) const
		{
			return mProperty->GetAccessFlags();
		}

		/// Returns true if the properties' values are equal. The properties must be of the same type.
		inline bool IsEqual(const PropertyHolder propertyToCopy)
		{
			return mProperty->IsEqual(mOwner, propertyToCopy.mOwner, propertyToCopy.mProperty);
		}

		/// Copies data from the specified abstract property. The property must be of the same type as this property.
		inline void CopyFrom(const PropertyHolder propertyToCopy)
		{
			mProperty->CopyFrom(mOwner, propertyToCopy.mOwner, propertyToCopy.mProperty);
		}

		/// Returns the typed value of this property.
		template<class T>
		T GetValue(void)
		{
			if (!mProperty)
			{
				ReportUndefined();
				return PropertyTypes::GetDefaultValue<T>();
			}
			return mProperty->GetValue<T>(mOwner);
		}

		/// Sets the typed value of this property.
		template<class T>
		void SetValue(const T value)
		{
			if (!mProperty)
			{
				ReportUndefined();
				return;
			}
			mProperty->SetValue<T>(mOwner, value);
		}

		/// Parses the typed value of this property from the input string.
		inline void SetValueFromString(const string& str)
		{
			mProperty->SetValueFromString(mOwner, str);
		}

		/// Calls a function this property represents.
		inline void CallFunction(PropertyFunctionParameters* parameters)
		{
			OC_DASSERT(parameters);
			OC_DASSERT(GetType() == PT_FUNCTION_PARAMETER);
			mProperty->SetValue<PropertyFunctionParameters>(mOwner, *parameters);
		}

		/// Calls a function this property represents.
		inline void CallFunction(PropertyFunctionParameters& parameters)
		{
			CallFunction(&parameters);
		}

		/// Returns the type of this property.
		inline ePropertyType GetType(void) const 
		{
			OC_DASSERT(mProperty);
			return mProperty->GetType();
		}

		/// Returns the name of this property.
		inline const char* GetName(void) const
		{
			OC_DASSERT(mProperty);
			return mProperty->GetName();
		}

		/// Returns true if this holder holds a valid property.
		inline bool IsValid(void) const { return (mOwner!=0 && mProperty!=0); }

	private:

		RTTIBaseClass* mOwner;
		AbstractProperty* mProperty;

		/// Problem reporting.
		void ReportUndefined(void);
	};
}

#endif // PropertyHolder_h__