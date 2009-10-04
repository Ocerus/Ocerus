#include "Common.h"
#include "PropertyFunctionParameters.h"

PropertyFunctionParameters PropertyFunctionParameters::None(0);

Reflection::PropertyFunctionParameters::PropertyFunctionParameters( int32 numParameters ):
	mParametersCount(0),
	mParametersMaxCount(numParameters)
{
	mParameters = new void*[numParameters];
}

void Reflection::PropertyFunctionParameters::Reset( int32 numParameters )
{
	if (mParameters)
		delete[] mParameters;
	mParametersCount = 0;
	mParametersMaxCount = numParameters;
	mParameters = new void*[numParameters];
}

Reflection::PropertyFunctionParameters::~PropertyFunctionParameters( void )
{
	BS_DASSERT(mParameters);
	delete[] mParameters;
};
