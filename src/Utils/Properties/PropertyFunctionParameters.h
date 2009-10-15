/// @file
/// Representation of generic parameters passed to a function accessed through the properties system.

#ifndef PropertyFunctionParameters_h__
#define PropertyFunctionParameters_h__

#include "Base.h"
#include "Array.h"

namespace Reflection
{
	/// A single parameter which can be passed to a function accessed through the properties system.
	typedef void* PropertyFunctionParameter;

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

		/// Reinitializes the parameters.
		void Reset(int32 numParameters);

		/// Destroys the parameters and deallocates used memory.
		~PropertyFunctionParameters(void);

		/// Adds a parameter to the end of the list.
		inline void PushParameter(PropertyFunctionParameter toAdd)
		{
			BS_DASSERT(!mParameters.IsNull());
			BS_DASSERT_MSG(mParametersCount < mParametersMaxCount, "Too many parameters added!");
			(*mParameters)[mParametersCount++] = toAdd;
		}

		/// Returns a parameter specified by the index.
		inline PropertyFunctionParameter GetParameter(int32 index) const
		{
			BS_DASSERT(!mParameters.IsNull());
			BS_DASSERT_MSG(index>=0 && index<mParametersCount, "Parameter index out of bounds!");
			return (*mParameters)[index];
		}

	private:

		int32 mParametersCount;
		int32 mParametersMaxCount;
		SmartPointer< Array<PropertyFunctionParameter> > mParameters;
	};
}

#endif // PropertyFunctionParameters_h__