/// @file
/// Representation of generic parameters passed to a function accessed through the properties system.

#ifndef PropertyFunctionParameters_h__
#define PropertyFunctionParameters_h__

#include "Base.h"

namespace Reflection
{
	/// This class represents generic parameters passed to a function accessed through the properties system.
	class PropertyFunctionParameters
	{
	public:

		/// No parameters.
		static PropertyFunctionParameters None;

		/// Constructs new parameters and allocates necessary memory for them. Maximum number of parameter is required.
		PropertyFunctionParameters(int32 numParameters);

		/// Reinitializes the parameters.
		void Reset(int32 numParameters);

		/// Destroys the parameters and deallocates used memory.
		~PropertyFunctionParameters(void);

		/// Adds a parameter to the end of the list.
		inline void PushParameter(void* toAdd)
		{
			BS_DASSERT_MSG(mParametersCount < mParametersMaxCount, "Too many parameters added!");
			mParameters[mParametersCount++] = toAdd;
		}

		/// Returns a parameter specified by the index.
		inline void* GetParameter(int32 index) const
		{
			BS_DASSERT_MSG(index>=0 && index<mParametersCount, "Parameter index out of bounds!");
			return mParameters[index];
		}

	private:

		int32 mParametersCount;
		int32 mParametersMaxCount;
		void** mParameters;

		/// Disabled to prevent unwanted memory copies.
		PropertyFunctionParameters(const PropertyFunctionParameters& rhs);
		PropertyFunctionParameters& operator=(const PropertyFunctionParameters& rhs);

	};
}

#endif // PropertyFunctionParameters_h__