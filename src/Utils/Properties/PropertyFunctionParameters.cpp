#include "Common.h"
#include "PropertyFunctionParameters.h"

PropertyFunctionParameters PropertyFunctionParameters::None(0);

Reflection::PropertyFunctionParameters::PropertyFunctionParameters( int32 numParameters )
{
	Reset(numParameters);
}

Reflection::PropertyFunctionParameters::PropertyFunctionParameters( void ):
	mParametersCount(0),
	mParametersMaxCount(0),
	mParameters()
{

}

Reflection::PropertyFunctionParameters::PropertyFunctionParameters( const PropertyFunctionParameters& rhs )
{
	*this = rhs;
}

PropertyFunctionParameters& Reflection::PropertyFunctionParameters::operator=( const PropertyFunctionParameters& rhs )
{
	mParameters = rhs.mParameters;
	mParametersCount = rhs.mParametersCount;
	mParametersMaxCount = rhs.mParametersMaxCount;
	return *this;
}

void Reflection::PropertyFunctionParameters::Reset( int32 numParameters )
{
	mParametersCount = 0;
	mParametersMaxCount = numParameters;
	mParameters = boost::shared_ptr< Array<PropertyFunctionParameter> >(new Array<PropertyFunctionParameter>(numParameters));
}

Reflection::PropertyFunctionParameters::~PropertyFunctionParameters( void )
{
	// we are not deleting the parameters as they'll be deleted automatically by the smart pointer
};

