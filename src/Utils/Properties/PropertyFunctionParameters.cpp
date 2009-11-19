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

Reflection::PropertyFunctionParameters& Reflection::PropertyFunctionParameters::operator=(const Reflection::PropertyFunctionParameters& rhs)
{
	if (this != &rhs)
		mParameters = rhs.mParameters; 
	return *this;
}

bool Reflection::PropertyFunctionParameters::operator==( const PropertyFunctionParameters& rhs )
{
	return mParameters == rhs.mParameters;
}
