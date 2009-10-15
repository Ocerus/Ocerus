/// @file
/// Definitions of individual value type of properties.
/// @remarks
/// Implementation based on the code from Game Programming Gems 5 (1.4).

#include "Common.h"
#include "PropertyEnums.h"

namespace Reflection
{

#ifdef __LINUX__
	template<class T> ePropertyType PropertyType<T>::mTypeID = PT_UNKNOWN;
	template<class T> T PropertyType<T>::mDefaultValue = 0;

	/// Template specialization.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue) \
		template<> ePropertyType PropertyType<typeClass>::mTypeID = typeID; \
		template<> typeClass PropertyType<typeClass>::mDefaultValue(defaultValue);
	#include "PropertyTypes.h"
	#undef PROPERTY_TYPE
#endif

}

