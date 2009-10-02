#ifndef PropertyFunctionParameters_h__
#define PropertyFunctionParameters_h__

#include "Settings.h"

class PropertyFunctionParameters
{
public:

	static PropertyFunctionParameters None;

	inline PropertyFunctionParameters(int32 numParameters):
		mParametersCount(0),
		mParametersMaxCount(numParameters)
	{
		mParameters = DYN_NEW void*[numParameters];
	};

	~PropertyFunctionParameters(void)
	{
		BS_DASSERT(mParameters);
		DYN_DELETE_ARRAY mParameters;
	};

	inline void PushParameter(void* toAdd)
	{
		BS_DASSERT_MSG(mParametersCount < mParametersMaxCount, "Too many parameters added!");
		mParameters[mParametersCount++] = toAdd;
	}

	inline void* GetParameter(int32 index) const
	{
		BS_DASSERT_MSG(index>=0 && index<mParametersCount, "Parameter index out of bounds!");
		return mParameters[index];
	}

private:

	int32 mParametersCount;
	int32 mParametersMaxCount;
	void** mParameters;

	PropertyFunctionParameters(const PropertyFunctionParameters& rhs);
	PropertyFunctionParameters& operator=(const PropertyFunctionParameters& rhs);

};

#endif // PropertyFunctionParameters_h__