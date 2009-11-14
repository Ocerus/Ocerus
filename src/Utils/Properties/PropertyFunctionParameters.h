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
	public:
		/// Constructs a null PropertyFunctionParameter.
		PropertyFunctionParameter(): mType(PT_UNKNOWN), mData(0)
		{
		}

		/// Constructs a PropertyFunctionParameter that will hold given property.
		template<class T>
		PropertyFunctionParameter(const T& property): mType(PropertyTypes::GetTypeID<T>()), mData(&property)
		{
		}

		/// Sets the PropertyFunctionParameter to given property.
		template<class T>
		void SetData(const T& property)
		{
			mType = PropertyTypes::GetTypeID<T>();
			mData = &property;
		}

		/// Returns the property.
		template<class T>
		const T* GetData() const
		{
			if (PropertyTypes::GetTypeID<T>() != mType)
			{
				//ocInfo << "PropertyFunctionParameter: calling `const " <<
				//		PropertyTypes::GetTypeID<T>() << "* GetData() const'" <<
				//		" for PropertyFunctionParameter containing `" << mType;

				return 0;
			}
			return static_cast<T>(mData);
		}

	private:
		ePropertyType   mType;
		const void*     mData;
	};

	/// This class represents generic parameters passed to a function accessed through the properties system.
	class PropertyFunctionParameters
	{
	public:

		/// No parameters.
		static PropertyFunctionParameters None;

		/// Constructs new empty parameters. No parameter will be allowed to add when using this constructor.
		PropertyFunctionParameters(void);

		/// Constructs new parameters and allocates necessary memory for them. Maximum number of parameter is required.
		PropertyFunctionParameters(int32 numParameters);

		/// Copy constructor.
		PropertyFunctionParameters(const PropertyFunctionParameters& rhs);

		/// Assignment operator.
		PropertyFunctionParameters& operator=(const PropertyFunctionParameters& rhs);

		/// Comparison operator.
		bool operator==(const PropertyFunctionParameters& rhs);

		/// Reinitializes the parameters.
		void Reset(int32 numParameters);

		/// Destroys the parameters and deallocates used memory.
		~PropertyFunctionParameters(void);

		/// Adds a parameter to the end of the list.
		inline void PushParameter(PropertyFunctionParameter toAdd)
		{
			OC_DASSERT(mParameters != 0);
			OC_DASSERT_MSG(mParametersCount < mParametersMaxCount, "Too many parameters added!");
			(*mParameters)[mParametersCount++] = toAdd;
		}

		/// Returns a parameter specified by the index.
		inline PropertyFunctionParameter GetParameter(int32 index) const
		{
			OC_DASSERT(mParameters != 0);
			OC_DASSERT_MSG(index>=0 && index<mParametersCount, "Parameter index out of bounds!");
			return (*mParameters)[index];
		}

		/// Returns the number of actual parameters inserted into this parameter holder.
		inline int32 GetParametersCount(void) const { return mParametersCount; }

	private:

		int32 mParametersCount;
		int32 mParametersMaxCount;
		boost::shared_ptr< Array<PropertyFunctionParameter> > mParameters;
	};
}

#endif // PropertyFunctionParameters_h__