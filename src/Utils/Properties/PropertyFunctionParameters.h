/// @file
/// Representation of generic parameters passed to a function accessed through the properties system.

#ifndef PropertyFunctionParameters_h__
#define PropertyFunctionParameters_h__

#include "Base.h"
#include "Array.h"
#include "PropertyEnums.h"

namespace Reflection
{

	/// A single parameter which can be passed to a function accessed through the properties system.
	class PropertyFunctionParameter
	{
		struct NullDeleter
		{
			void operator()(void const*) const
			{
			}
		};
	public:

		/// Constructs a null PropertyFunctionParameter.
		PropertyFunctionParameter(): mType(PT_UNKNOWN)
		{
		}

		/// Constructs a PropertyFunctionParameter that will hold a copy of given property.
		/// Data are copied.
		template<class T>
		PropertyFunctionParameter(const T& property):
			mType(PropertyTypes::GetTypeID<T>()),
			mData(new T(property))
		{
		}

		/// Constructs a PropertyFunctionParameter that will hold a reference to given property.
		/// Data are not copied.
		template<class T>
		PropertyFunctionParameter(T& property, bool):
			mType(PropertyTypes::GetTypeID<T>()),
			mData(&property, NullDeleter())
		{
		}

		/// Returns the property.
		template<class T>
		T* GetData() const
		{
			if (PropertyTypes::GetTypeID<T>() != mType)
			{
				return 0;
			}
			return &*boost::static_pointer_cast<T>(mData);
		}

		/// Returns the type of this property.
		inline ePropertyType GetType(void) const { return mType; }

	private:
		ePropertyType           mType;
		boost::shared_ptr<void> mData;
	};

	/// This class represents generic parameters passed to a function accessed through the properties system.
	class PropertyFunctionParameters
	{
	public:
		/// Empty parameters.
		static PropertyFunctionParameters Null;

		/// Constructs new empty parameters.
		PropertyFunctionParameters();

		/// Destroys the parameters and deallocates used memory.
		~PropertyFunctionParameters(void);

		/// Assignment operator.
		PropertyFunctionParameters& operator=(const PropertyFunctionParameters& rhs);

		/// Comparison operator.
		bool operator==(const PropertyFunctionParameters& rhs);

		/// Adds a parameter to the end of the list.
		inline void PushParameter(const PropertyFunctionParameter& toAdd)
		{
			mParameters->push_back(toAdd);
		}

		/// This method is provided for convenience and allows to create a PropertyFunctionParameters
		/// object and fill it with parameters with following code:
		///
		/// (PropertyFunctionParameters() << param1 << param2;
		inline PropertyFunctionParameters operator<<(const PropertyFunctionParameter& toAdd) const
		{
			PropertyFunctionParameters res(*this);
			res.mParameters->push_back(toAdd);
			return res;
		}

		/// Returns a parameter specified by the index. If index out of range is
		/// supplied, returns null parameter.
		inline PropertyFunctionParameter GetParameter(uint32 index) const
		{
			if (index >= mParameters->size())
			{
				ocError << "Out of parameter bounds";
				return PropertyFunctionParameter();
			}
			return mParameters->at(index);
		}

		/// Returns the number of actual parameters inserted into this parameter holder.
		inline uint32 GetParametersCount(void) const { return mParameters->size(); }

	private:

		boost::shared_ptr< vector<PropertyFunctionParameter> > mParameters;
	};
}

#endif // PropertyFunctionParameters_h__