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

        /// Comparsion operator.
        bool operator==(const PropertyHolder& rhs)
        {
            return rhs.mOwner == mOwner && rhs.mProperty == mProperty;
        }

		/// Returns the access flags to this property.
		inline PropertyAccessFlags GetAccessFlags(void) const
		{
			OC_DASSERT(mProperty);
			return mProperty->GetAccessFlags();
		}

		/// Returns the key representing this property.
		inline StringKey GetKey(void) const
		{
			OC_DASSERT(mProperty);
			return mProperty->GetKey();
		}

		/// Returns true if the properties' values are equal. The properties must be of the same type.
		inline bool IsEqual(const PropertyHolder propertyToCopy)
		{
			OC_DASSERT(mProperty);
			return mProperty->IsEqual(mOwner, propertyToCopy.mOwner, propertyToCopy.mProperty);
		}

		/// Copies data from the specified abstract property. The property must be of the same type as this property.
		inline void CopyFrom(const PropertyHolder propertyToCopy)
		{
			OC_DASSERT(mProperty);
			mProperty->CopyFrom(mOwner, propertyToCopy.mOwner, propertyToCopy.mProperty);
		}

		/// Returns the typed value of this property.
		template<class T>
		T GetValue(void) const
		{
			if (!mProperty)
			{
				ReportUndefined();
				return PropertyTypes::GetDefaultValue<T>();
			}
			return mProperty->GetValue<T>(mOwner);
		}

		inline string GetValueString(void) const
		{
			if (!mProperty)
			{
				ReportUndefined();
				return "";
			}
			return mProperty->GetValueString(mOwner);
		}

		/// Write the XML representation of the value of holder's property to XML output.
		inline void WriteValueXML(ResourceSystem::XMLOutput& output)
		{
			if (!mProperty)
			{
				ReportUndefined();
				return;
			}
			mProperty->WriteValueXML(mOwner, output);
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
			if (!mProperty)
			{
				ReportUndefined();
				return;
			}
			mProperty->SetValueFromString(mOwner, str);
		}
		
		/// Parses the typed valued of this property from the XML input.
		inline void ReadValueXML(ResourceSystem::XMLNodeIterator& input)
		{
			if (!mProperty)
			{
				ReportUndefined();
				return;
			}
			mProperty->ReadValueXML(mOwner, input);
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

		/// Returns the comment of this property or empty string if no comment was set.
		inline string GetComment(void) const
		{
			OC_DASSERT(mProperty);
			return mProperty->GetComment();
		}

		/// Returns true if this holder holds a valid property.
		inline bool IsValid(void) const { return (mOwner!=0 && mProperty!=0); }

		/// Returns true if this holder holds property that holds its value.
		inline bool IsValued(void) const
		{
			OC_DASSERT(mProperty);
			return mProperty->IsValued();
		}

	private:

		RTTIBaseClass* mOwner;
		AbstractProperty* mProperty;

		/// Problem reporting.
		void ReportUndefined(void) const;
	};
}

#endif // PropertyHolder_h__
