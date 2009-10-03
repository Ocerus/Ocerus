#include "Common.h"
#include "PropertyFunctionParameters.h"

PropertyFunctionParameters PropertyFunctionParameters::None(0);

Reflection::PropertyFunctionParameters::PropertyFunctionParameters( int32 numParameters ):
	mParametersCount(0),
	mParametersMaxCount(numParameters)
{
	mParameters = DYN_NEW void*[numParameters];
}

void Reflection::PropertyFunctionParameters::Reset( int32 numParameters )
{
	if (mParameters)
		DYN_DELETE_ARRAY mParameters;
	mParametersCount = 0;
	mParametersMaxCount = numParameters;
	mParameters = DYN_NEW void*[numParameters];
}