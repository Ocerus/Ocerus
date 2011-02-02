#include "Common.h"
#include "PropertyHolder.h"


void PropertyHolder::ReportUndefined( void ) const
{
	ocError << "PropertyHolder: Undefined property";
}

#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning)\
template<> \
bool PropertyHolder::IsEqualToArray<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(ARRAY_PROPERTY_TYPE_CLASS(typeClass) other) const \
{ \
	ARRAY_PROPERTY_TYPE_CLASS(typeClass) arrayValue = GetValue<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(); \
	if (arrayValue) return *arrayValue == *other; \
	else return 0; \
}
#include "PropertyTypes.h"
#undef PROPERTY_TYPE
