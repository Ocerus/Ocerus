#include "Common.h"
#include "PropertyFunctionParameters.h"

Reflection::PropertyFunctionParameters PropertyFunctionParameters::Null;

Reflection::PropertyFunctionParameters::PropertyFunctionParameters():
	mParameters(new vector<PropertyFunctionParameter>())
{
}

Reflection::PropertyFunctionParameters::~PropertyFunctionParameters( void )
{
}

bool Reflection::PropertyFunctionParameters::operator==( const PropertyFunctionParameters& rhs )
{
	return mParameters == rhs.mParameters;
}

PropertyFunctionParameters& Reflection::PropertyFunctionParameters::operator=(const PropertyFunctionParameters& rhs)
{
    mParameters = rhs.mParameters;
    return *this;
}
